"""
Title: End-to-end Masked Language Modeling with BERT
Author: [Ankur Singh](https://twitter.com/ankur310794)
Date created: 2020/09/18
Last modified: 2024/03/15
Description: Implement a Masked Language Model (MLM) with BERT and fine-tune it on the IMDB Reviews dataset.
Accelerator: GPU
Converted to Keras 3 by: [Sitam Meur](https://github.com/sitamgithub-MSIT)
"""

"""
## Introduction

Masked Language Modeling is a fill-in-the-blank task,
where a model uses the context words surrounding a mask token to try to predict what the
masked word should be.

For an input that contains one or more mask tokens,
the model will generate the most likely substitution for each.

Example:

- Input: "I have watched this [MASK] and it was awesome."
- Output: "I have watched this movie and it was awesome."

Masked language modeling is a great way to train a language
model in a self-supervised setting (without human-annotated labels).
Such a model can then be fine-tuned to accomplish various supervised
NLP tasks.

This example teaches you how to build a BERT model from scratch,
train it with the masked language modeling task,
and then fine-tune this model on a sentiment classification task.

We will use the Keras `MultiHeadAttention` layers
to create a BERT Transformer-Encoder network architecture.

Note: This example should be run with `tf-nightly`.
"""

"""
## Setup

Install `tf-nightly` via `pip install tf-nightly`.
"""

import os

os.environ["KERAS_BACKEND"] = "jax"  # or tensorflow, torch

import keras_hub

import keras
from keras import layers, ops
from keras.layers import TextVectorization

from dataclasses import dataclass
import pandas as pd
import numpy as np
import glob
import re
from pprint import pprint
import tree

"""
## Set-up Configuration
"""


@dataclass
class Config:
    MAX_LEN = 256
    BATCH_SIZE = 32
    LR = 0.001
    VOCAB_SIZE = 30000
    EMBED_DIM = 128
    NUM_HEAD = 8  # used in bert model
    FF_DIM = 128  # used in bert model
    NUM_LAYERS = 1


config = Config()

"""
## Load the data

We will first download the IMDB data and load into a Pandas dataframe.
"""

"""shell
curl -O https://ai.stanford.edu/~amaas/data/sentiment/aclImdb_v1.tar.gz
tar -xf aclImdb_v1.tar.gz
"""


def get_text_list_from_files(files):
    text_list = []
    for name in files:
        with open(name) as f:
            for line in f:
                text_list.append(line)
    return text_list


def get_data_from_text_files(folder_name):
    pos_files = glob.glob("aclImdb/" + folder_name + "/pos/*.txt")
    pos_texts = get_text_list_from_files(pos_files)
    neg_files = glob.glob("aclImdb/" + folder_name + "/neg/*.txt")
    neg_texts = get_text_list_from_files(neg_files)
    df = pd.DataFrame(
        {
            "review": pos_texts + neg_texts,
            "sentiment": [0] * len(pos_texts) + [1] * len(neg_texts),
        }
    )
    df = df.sample(len(df)).reset_index(drop=True)
    return df


train_df = get_data_from_text_files("train").iloc[0:500]
test_df = get_data_from_text_files("test").iloc[0:250]

all_data = train_df._append(test_df)

"""
## Dataset preparation

We will use the `TextVectorization` layer to vectorize the text into integer token ids.
It transforms a batch of strings into either
a sequence of token indices (one sample = 1D array of integer token indices, in order)
or a dense representation (one sample = 1D array of float values encoding an unordered set of tokens).

Below, we define 3 preprocessing functions.

1.  The `get_vectorize_layer` function builds the `TextVectorization` layer.
2.  The `get_vocabulary` function that returns a vocabulary.
3.  The `get_masked_input_and_labels` function will mask input token ids.
It masks 15% of all input tokens in each sequence at random.
"""

# Required for tf.data.Dataset and its pre-processing function
import tensorflow as tf


def custom_standardization(input_data):
    lowercase = tf.strings.lower(input_data)
    stripped_html = tf.strings.regex_replace(lowercase, "<br />", " ")
    return tf.strings.regex_replace(
        stripped_html, "[%s]" % re.escape("!#$%&'()*+,-./:;<=>?@\^_`{|}~"), ""
    )


"""
## Vocabulary Estimation

From the `TextVectorization` [documentation](), it is recommended that we
implement `TextVectorization` inside the `tf.data.Dataset`.We have tried
to approximate the logic behind the TextVectorizer `adapt` method by creating
the `get_vocabulary` function` - this function returns a vocabulary. We needed
this vocabulary beacuse when trying to run adapt inside tf.data.Datset it 
returns the error [](), and still after several fixes the error remians.
"""

# Define a function to calculate the vocabulary
def get_vocabulary(dataframe):
    """
    Args: dataframe-like object
    Returns: vocabulary (a list of tokens)
    """

    data = tf.cast(dataframe.review.values.tolist(), dtype="string")
    # Apply the above custom_standardization function
    data = custom_standardization(data)
    # Split the string data on whitespace
    vocabulary = tf.strings.split(data)
    # Flatten the RaggedTensor data
    flat_values = vocabulary.flat_values
    # Returns the unique tokens
    tokens, _, counts = tf.unique_with_counts(flat_values, out_idx="int64")
    # This class is the HashTable we use to store our tokens
    token_counts = tf.lookup.experimental.MutableHashTable(
        key_dtype="string",
        value_dtype="int64",
        default_value=0,
    )
    token_counts.insert(tokens, counts + token_counts.lookup(tokens))
    tokens, counts = token_counts.export()
    # To keep vocabs deterministic, we sort our tokens by count and break
    # ties by sorting the tokens themselves. Tensorflow has no ops for
    # sorting strings, so we need to use numpy for the sort.
    sorted_indices = np.lexsort((tokens.numpy(), counts.numpy()))[::-1]
    # No special_tokens inserted in our vocab
    token_start = 0
    # Truncate the vocab
    max_learned_tokens = config.VOCAB_SIZE - token_start
    sorted_indices = sorted_indices[:max_learned_tokens]
    tokens = tf.gather(tokens, sorted_indices)
    tokens = [
        i.decode("utf-8") if isinstance(i, bytes) else i
        for i in tokens.numpy().tolist()
    ]
    # Insert [mask] as a special token
    vocabulary = (
        tokens[2 : len(tokens) - len(["[mask]"])] + ["[mask]"]
    )
    return vocabulary


# Metadata
FEATURES_WITH_VOCABULARY = {"review": get_vocabulary(all_data)}

id2token = dict(enumerate(FEATURES_WITH_VOCABULARY["review"]))
token2id = {y: x for x, y in id2token.items()}


# Define a function that vectorizes the text
def get_vectorize_layer(features, target):
    """Build Text vectorization layer"""

    for feature_name in features:
        if feature_name in FEATURES_WITH_VOCABULARY:
            vectorize_layer = TextVectorization(
                output_mode="int",
                vocabulary=FEATURES_WITH_VOCABULARY["review"],
                output_sequence_length=config.MAX_LEN,
            )
            vectorize_layer.get_vocabulary()
            value_index = vectorize_layer(features[feature_name])
            features[feature_name] = value_index
        else:
            pass
    return dict(features), target

# Define a function that retruns a dataset
def get_dataset(x_train):
    if hasattr(x_train, "sentiment"):
        labels = x_train.copy().pop("sentiment")
    else:
        labels = None
    # vectorized_text = get_vectorize_layer(x_train)
    train_classifier_ds = tf.data.Dataset.from_tensor_slices(
        (dict(x_train), labels)
    ).map(get_vectorize_layer)
    return train_classifier_ds


# Get mask token id
mask_token_id = FEATURES_WITH_VOCABULARY["review"].index("[mask]")

def get_masked_input_and_labels(encoded_texts):
    # 15% BERT masking
    inp_mask = np.random.rand(*encoded_texts.shape) < 0.15
    # Do not mask special tokens
    inp_mask[encoded_texts <= 2] = False
    # Set targets to -1 by default, it means ignore
    labels = -1 * np.ones(encoded_texts.shape, dtype=int)
    # Set labels for masked tokens
    labels[inp_mask] = encoded_texts[inp_mask]

    # Prepare input
    encoded_texts_masked = np.copy(encoded_texts)
    # Set input to [MASK] which is the last token for the 90% of tokens
    # This means leaving 10% unchanged
    inp_mask_2mask = inp_mask & (np.random.rand(*encoded_texts.shape) < 0.90)
    encoded_texts_masked[inp_mask_2mask] = (
        mask_token_id  # mask token is the last in the dict
    )

    # Set 10% to a random token
    inp_mask_2random = inp_mask_2mask & (np.random.rand(*encoded_texts.shape) < 1 / 9)
    encoded_texts_masked[inp_mask_2random] = np.random.randint(
        3, mask_token_id, inp_mask_2random.sum()
    )

    # Prepare sample_weights to pass to .fit() method
    sample_weights = np.ones(labels.shape)
    sample_weights[labels == -1] = 0

    # y_labels would be same as encoded_texts i.e input tokens
    y_labels = np.copy(encoded_texts)

    return encoded_texts_masked, y_labels, sample_weights


# We have 25000 examples for training
train_classifier_ds = get_dataset(train_df).shuffle(1000).batch(config.BATCH_SIZE)

# We have 25000 examples for testing
test_classifier_ds = get_dataset(test_df).shuffle(1000).batch(config.BATCH_SIZE)

# Build dataset for end to end model input (will be used at the end)
test_raw_classifier_ds = test_df

# Prepare data for masked language model
# all_data.loc[len(all_data)] = {"review": "I have watched this [mask] and it was awesome", "sentiment":1}
x_all_review = np.array([x[0]["review"] for x in get_dataset(all_data)])
x_masked_train, y_masked_labels, sample_weights = get_masked_input_and_labels(
    x_all_review
)

mlm_ds = tf.data.Dataset.from_tensor_slices(
    (x_masked_train, y_masked_labels, sample_weights)
)
mlm_ds = mlm_ds.shuffle(1000).batch(config.BATCH_SIZE)

"""
## Create BERT model (Pretraining Model) for masked language modeling

We will create a BERT-like pretraining model architecture
using the `MultiHeadAttention` layer.
It will take token ids as inputs (including masked tokens)
and it will predict the correct ids for the masked input tokens.
"""


def bert_module(query, key, value, i):
    # Multi headed self-attention
    attention_output = layers.MultiHeadAttention(
        num_heads=config.NUM_HEAD,
        key_dim=config.EMBED_DIM // config.NUM_HEAD,
        name="encoder_{}_multiheadattention".format(i),
    )(query, key, value)
    attention_output = layers.Dropout(0.1, name="encoder_{}_att_dropout".format(i))(
        attention_output
    )
    attention_output = layers.LayerNormalization(
        epsilon=1e-6, name="encoder_{}_att_layernormalization".format(i)
    )(query + attention_output)

    # Feed-forward layer
    ffn = keras.Sequential(
        [
            layers.Dense(config.FF_DIM, activation="relu"),
            layers.Dense(config.EMBED_DIM),
        ],
        name="encoder_{}_ffn".format(i),
    )
    ffn_output = ffn(attention_output)
    ffn_output = layers.Dropout(0.1, name="encoder_{}_ffn_dropout".format(i))(
        ffn_output
    )
    sequence_output = layers.LayerNormalization(
        epsilon=1e-6, name="encoder_{}_ffn_layernormalization".format(i)
    )(attention_output + ffn_output)
    return sequence_output


loss_fn = keras.losses.SparseCategoricalCrossentropy(reduction=None)
loss_tracker = keras.metrics.Mean(name="loss")


class MaskedLanguageModel(keras.Model):

    def train_step(self, *args, **kwargs):
        if keras.backend.backend() == "jax":
            return self._jax_train_step(*args, **kwargs)
        elif keras.backend.backend() == "tensorflow":
            return self._tensorflow_train_step(*args, **kwargs)
        elif keras.backend.backend() == "torch":
            return self._torch_train_step(*args, **kwargs)

    # Method used by jax backend
    def compute_loss_and_updates(
        self,
        trainable_variables,
        non_trainable_variables,
        features,
        labels,
        sample_weight,
        training=False,
    ):
        y_pred, non_trainable_variables = self.stateless_call(
            trainable_variables,
            non_trainable_variables,
            features,
            training=training,
        )
        loss = loss_fn(labels, y_pred, sample_weight=sample_weight)
        summed_loss = loss.sum()
        return summed_loss, (y_pred, non_trainable_variables, loss)

    def _jax_train_step(self, state, inputs):

        import jax

        (
            trainable_variables,
            non_trainable_variables,
            optimizer_variables,
            metrics_variables,
        ) = state
        if len(inputs) == 3:
            features, labels, sample_weight = inputs
        else:
            features, labels = inputs
            sample_weight = None
        # Get the gradient function.
        grad_fn = jax.value_and_grad(self.compute_loss_and_updates, has_aux=True)
        # Compute the gradients.
        (summed_loss, (y_pred, non_trainable_variables, loss)), grads = grad_fn(
            trainable_variables,
            non_trainable_variables,
            features,
            labels,
            sample_weight,
            training=True,
        )
        # Update trainable variables and optimizer variables.
        (
            trainable_variables,
            optimizer_variables,
        ) = self.optimizer.stateless_apply(
            optimizer_variables, grads, trainable_variables
        )
        # Compute our own metrics
        new_metrics_vars = []
        logs = {}
        for metric in self.metrics:
            this_metric_vars = metrics_variables[
                len(new_metrics_vars) : len(new_metrics_vars) + len(metric.variables)
            ]
            if metric.name == "loss":
                this_metric_vars = metric.stateless_update_state(
                    this_metric_vars, loss, sample_weight=sample_weight
                )
            else:
                this_metric_vars = metric.stateless_update_state(
                    this_metric_vars, labels, y_pred
                )
            logs[metric.name] = metric.stateless_result(this_metric_vars)
            new_metrics_vars += this_metric_vars
        # Return metric logs and updated state variables.
        state = (
            trainable_variables,
            non_trainable_variables,
            optimizer_variables,
            new_metrics_vars,
        )
        # Return a dict mapping metric names to current value, and
        # state for model saving
        return {"loss": logs["loss"]}, state

    def _tensorflow_train_step(self, inputs):

        if len(inputs) == 3:
            features, labels, sample_weight = inputs
        else:
            features, labels = inputs
            sample_weight = None
        with tf.GradientTape() as tape:
            predictions = self(features, training=True)
            loss = loss_fn(labels, predictions, sample_weight=sample_weight)
        # Compute gradients
        trainable_vars = self.trainable_variables
        gradients = tape.gradient(loss, trainable_vars)
        # Update weights
        self.optimizer.apply_gradients(zip(gradients, trainable_vars))
        # Compute our own metrics
        loss_tracker.update_state(loss, sample_weight=sample_weight)
        # Return a dict mapping metric names to current value
        return {"loss": loss_tracker.result()}

    def _torch_train_step(self, inputs):

        import torch

        if len(inputs) == 3:
            features, labels, sample_weight = inputs
        else:
            features, labels = inputs
            sample_weight = None
        self.zero_grad()
        # Compute loss
        predictions = self(features, training=True)
        loss = loss_fn(labels, predictions, sample_weight=sample_weight)
        # Call torch.Tensor.backward() on the loss to compute gradients
        # for the weights.
        loss.sum().backward()
        trainable_weights = [v for v in self.trainable_weights]
        gradients = [v.value.grad for v in trainable_weights]
        # Update weights
        with torch.no_grad():
            self.optimizer.apply(gradients, trainable_weights)
        # Compute our own metrics
        loss_tracker.update_state(loss, sample_weight=sample_weight)
        # Return a dict mapping metric names to current value
        return {"loss": loss_tracker.result()}

    @property
    def metrics(self):
        # We list our `Metric` objects here so that `reset_states()` can be
        # called automatically at the start of each epoch
        # or at the start of `evaluate()`.
        # If you don't implement this property, you have to call
        # `reset_states()` yourself at the time of your choosing.
        return [loss_tracker]


def create_masked_language_bert_model():
    inputs = layers.Input((config.MAX_LEN,), dtype="int32")
    # max_index = word_embeddings.input_dim - 1  # Clamp the indices
    word_embeddings = layers.Embedding(
        config.VOCAB_SIZE, config.EMBED_DIM, name="word_embedding"
    )
    word_embeddings = word_embeddings(
        keras.ops.clip(inputs, 0, (word_embeddings.input_dim - 1))
    )
    position_embeddings = keras_hub.layers.PositionEmbedding(
        sequence_length=config.MAX_LEN
    )(word_embeddings)
    embeddings = word_embeddings + position_embeddings
    encoder_output = embeddings
    for i in range(config.NUM_LAYERS):
        encoder_output = bert_module(encoder_output, encoder_output, encoder_output, i)
    mlm_output = layers.Dense(config.VOCAB_SIZE, name="mlm_cls", activation="softmax")(
        encoder_output
    )
    mlm_model = MaskedLanguageModel(inputs, mlm_output, name="masked_bert_model")
    optimizer = keras.optimizers.Adam(learning_rate=config.LR)
    mlm_model.compile(optimizer=optimizer)
    return mlm_model


class MaskedTextGenerator(keras.callbacks.Callback):
    def __init__(self, sample_tokens, top_k=5):
        self.sample_tokens = sample_tokens
        self.k = top_k

    def decode(self, tokens):
        return " ".join([id2token[t] for t in tokens if t != 0])

    def convert_ids_to_tokens(self, id):
        return id2token[id]

    def on_epoch_end(self, epoch, logs=None):

        prediction = self.model.predict(self.sample_tokens)
        masked_index = np.where(self.sample_tokens == mask_token_id)
        masked_index = masked_index[1]
        mask_prediction = prediction[0][masked_index]
        top_indices = mask_prediction[0].argsort()[-self.k :][::-1]
        values = mask_prediction[0][top_indices]
        for i in range(len(top_indices)):
            p = top_indices[i]
            v = values[i]
            tokens = np.copy(sample_tokens[0])
            tokens[masked_index[0]] = p
            result = {
                "input_text": self.decode(sample_tokens[0]),
                "prediction": self.decode(tokens),
                "probability": v,
                "predicted mask token": self.convert_ids_to_tokens(p),
            }
            pprint(result)

sample_tokens = "I have watched this [mask] and it was awesome"
tokens_ids = [
    token2id.get(x, 0)
    for x in sample_tokens.split()
]
# Pad the tokens list with zeros if necessary to make it 256 in length
tokens_ids = tokens_ids[: config.MAX_LEN] + [
    0
] * (config.MAX_LEN - len(tokens_ids))
# Convert the list to a tensor (with the batch size as 1)
sample_tokens = np.reshape(tokens_ids, (1, -1))
generator_callback = MaskedTextGenerator(sample_tokens)
bert_masked_model = create_masked_language_bert_model()
bert_masked_model.summary()

"""
## Train and Save
"""

bert_masked_model.fit(mlm_ds, epochs=1, callbacks=[generator_callback])
bert_masked_model.save("bert_mlm_imdb.keras")

"""
## Fine-tune a sentiment classification model

We will fine-tune our self-supervised model on a downstream task of sentiment classification.
To do this, let's create a classifier by adding a pooling layer and a `Dense` layer on top of the
pretrained BERT features.

"""

# Load pretrained bert model
mlm_model = keras.models.load_model(
    "bert_mlm_imdb.keras", custom_objects={"MaskedLanguageModel": MaskedLanguageModel}
)
pretrained_bert_model = keras.Model(
    mlm_model.input, mlm_model.get_layer("encoder_0_ffn_layernormalization").output
)

# Freeze it
pretrained_bert_model.trainable = False


def create_classifier_bert_model():
    inputs = layers.Input((config.MAX_LEN,), dtype="int32", name="review")
    sequence_output = pretrained_bert_model(inputs)
    pooled_output = layers.GlobalMaxPooling1D()(sequence_output)
    hidden_layer = layers.Dense(64, activation="relu")(pooled_output)
    outputs = layers.Dense(1, activation="sigmoid")(hidden_layer)
    classifer_model = keras.Model(inputs, outputs, name="classification")
    optimizer = keras.optimizers.Adam()
    classifer_model.compile(
        optimizer=optimizer, loss="binary_crossentropy", metrics=["accuracy"]
    )
    return classifer_model


classifer_model = create_classifier_bert_model()
classifer_model.summary()

# Train the classifier with frozen BERT stage
classifer_model.fit(
    train_classifier_ds,
    epochs=1,
    validation_data=test_classifier_ds,
)

# Unfreeze the BERT model for fine-tuning
pretrained_bert_model.trainable = True
optimizer = keras.optimizers.Adam()
classifer_model.compile(
    optimizer=optimizer, loss="binary_crossentropy", metrics=["accuracy"]
)
classifer_model.fit(
    train_classifier_ds,
    epochs=1,
    validation_data=test_classifier_ds,
)

"""
## Create an end-to-end model and evaluate it

When you want to deploy a model, it's best if it already includes its preprocessing
pipeline, so that you don't have to reimplement the preprocessing logic in your
production environment. Let's create an end-to-end model that incorporates
the 'pre-processing', and let's evaluate. Our model will accept raw strings
as input.
"""

# We create a custom Model to override th evaluate method so
# that if first pre-process text data
class ModelEndtoEnd(keras.Model):

    def evaluate(self, inputs):
        indices = get_dataset(inputs).shuffle(1000).batch(config.BATCH_SIZE)
        return super().evaluate(indices)


def get_end_to_end(model):
    inputs = classifer_model.inputs
    outputs = classifer_model.outputs
    end_to_end_model = ModelEndtoEnd(inputs, outputs, name="end_to_end_model")
    optimizer = keras.optimizers.Adam(learning_rate=config.LR)
    end_to_end_model.compile(
        optimizer=optimizer, loss="binary_crossentropy", metrics=["accuracy"]
    )
    return end_to_end_model


end_to_end_classification_model = get_end_to_end(classifer_model)
end_to_end_classification_model.evaluate(test_raw_classifier_ds)


