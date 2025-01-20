
import os
import subprocess
import tarfile
from typing import Optional, Union
import tree

os.environ["KERAS_BACKEND"] = "tensorflow"  # or jax, or tensorflow

import numpy as np
import pandas as pd

# Some objects used are internal, not public
import keras
from keras import layers
from keras import losses
from keras import activations
# from keras.src.utils import traceback_utils
# from keras.src.trainers.data_adapters import data_adapter_utils
# from keras.src.backend.tensorflow.trainer import TFEpochIterator
from keras.src import callbacks as callbacks_module
from keras.src import optimizers as optimizers_module

import keras_tuner
from keras_tuner.engine import hyperparameters


import autokeras as ak
from autokeras.engine import io_hypermodel
from autokeras.engine import head as head_module
from autokeras.engine import node as node_module
from autokeras.utils import utils
from autokeras import adapters
from autokeras import analysers
from autokeras import hyper_preprocessors as hpps_module
from autokeras import preprocessors
from autokeras.blocks import reduction
from autokeras.engine import head as head_module
from autokeras.utils import types
from autokeras.utils import utils


"""
## Prepare the data

First we load the data from the UCI Machine Learning Repository into a Pandas DataFrame.
"""

# Column names.
CSV_HEADER = [
    "age",
    "class_of_worker",
    "detailed_industry_recode",
    "detailed_occupation_recode",
    "education",
    "wage_per_hour",
    "enroll_in_edu_inst_last_wk",
    "marital_stat",
    "major_industry_code",
    "major_occupation_code",
    "race",
    "hispanic_origin",
    "sex",
    "member_of_a_labor_union",
    "reason_for_unemployment",
    "full_or_part_time_employment_stat",
    "capital_gains",
    "capital_losses",
    "dividends_from_stocks",
    "tax_filer_stat",
    "region_of_previous_residence",
    "state_of_previous_residence",
    "detailed_household_and_family_stat",
    "detailed_household_summary_in_household",
    "instance_weight",
    "migration_code-change_in_msa",
    "migration_code-change_in_reg",
    "migration_code-move_within_reg",
    "live_in_this_house_1_year_ago",
    "migration_prev_res_in_sunbelt",
    "num_persons_worked_for_employer",
    "family_members_under_18",
    "country_of_birth_father",
    "country_of_birth_mother",
    "country_of_birth_self",
    "citizenship",
    "own_business_or_self_employed",
    "fill_inc_questionnaire_for_veterans_admin",
    "veterans_benefits",
    "weeks_worked_in_year",
    "year",
    "income_level",
]

data_url = "https://archive.ics.uci.edu/static/public/117/census+income+kdd.zip"
keras.utils.get_file(origin=data_url, extract=True)


"""
Determine the downloaded .tar.gz file path and
extract the files from the downloaded .tar.gz file
"""

extracted_path = os.path.join(
    os.path.expanduser("~"), ".keras", "datasets", "census+income+kdd.zip"
)
for root, dirs, files in os.walk(extracted_path):
    for file in files:
        if file.endswith(".tar.gz"):
            tar_gz_path = os.path.join(root, file)
            with tarfile.open(tar_gz_path, "r:gz") as tar:
                tar.extractall(path=root)

train_data_path = os.path.join(
    os.path.expanduser("~"),
    ".keras",
    "datasets",
    "census+income+kdd.zip",
    "census-income.data",
)
test_data_path = os.path.join(
    os.path.expanduser("~"),
    ".keras",
    "datasets",
    "census+income+kdd.zip",
    "census-income.test",
)

data = pd.read_csv(train_data_path, header=None, names=CSV_HEADER).iloc[0:10000]
test_data = pd.read_csv(test_data_path, header=None, names=CSV_HEADER).iloc[0:5000]

print(f"Data shape: {data.shape}")
print(f"Test data shape: {test_data.shape}")


"""
We convert the target column from string to integer.
"""

data["income_level"] = data["income_level"].apply(
    lambda x: 0 if x == " - 50000." else 1
)
test_data["income_level"] = test_data["income_level"].apply(
    lambda x: 0 if x == " - 50000." else 1
)


"""
Then, We split the dataset into train and validation sets.
"""

random_selection = np.random.rand(len(data.index)) <= 0.85
train_data = data[random_selection]
valid_data = data[~random_selection]


"""
Finally we store the train and test data splits locally to CSV files.
"""

train_data_file = "train_data.csv"
valid_data_file = "valid_data.csv"
test_data_file = "test_data.csv"

train_data.to_csv(train_data_file, index=False, header=False)
valid_data.to_csv(valid_data_file, index=False, header=False)
test_data.to_csv(test_data_file, index=False, header=False)


# clean the directory excpet the .tar.gz file
subprocess.run(
    f'find {extracted_path} -type f ! -name "*.tar.gz" -exec rm -f {{}} +',
    shell=True,
    check=True,
)
# remove empty directories
subprocess.run(
    f"find {extracted_path} -type d -empty -exec rmdir {{}} +", shell=True, check=True
)

"""
## Define dataset metadata

Here, we define the metadata of the dataset that will be useful for reading and
parsing the data into input features, and encoding the input features with respect
to their types.
"""

# Target feature name.
TARGET_FEATURE_NAME = "income_level"
# Weight column name.
WEIGHT_COLUMN_NAME = "instance_weight"
# Numeric feature names.
NUMERIC_FEATURE_NAMES = [
    "age",
    "wage_per_hour",
    "capital_gains",
    "capital_losses",
    "dividends_from_stocks",
    "num_persons_worked_for_employer",
    "weeks_worked_in_year",
]
# Categorical features and their vocabulary lists.
# Note that we add 'v=' as a prefix to all categorical feature values to make
# sure that they are treated as strings.
CATEGORICAL_FEATURES_WITH_VOCABULARY = {
    feature_name: sorted([str(value) for value in list(data[feature_name].unique())])
    for feature_name in CSV_HEADER
    if feature_name
    not in list(NUMERIC_FEATURE_NAMES + [WEIGHT_COLUMN_NAME, TARGET_FEATURE_NAME])
}
# All features names.
FEATURE_NAMES = NUMERIC_FEATURE_NAMES + list(
    CATEGORICAL_FEATURES_WITH_VOCABULARY.keys()
)
# Feature default values.
COLUMN_DEFAULTS = [
    (
        [0.0]
        if feature_name
        in NUMERIC_FEATURE_NAMES + [TARGET_FEATURE_NAME, WEIGHT_COLUMN_NAME]
        else ["NA"]
    )
    for feature_name in CSV_HEADER
]

"""
## Create a `tf.data.Dataset` for training and evaluation

We create an input function to read and parse the file, and convert features and
labels into a [`tf.data.Dataset`](https://www.tensorflow.org/guide/datasets) for
training and evaluation.
"""

# Tensorflow required for tf.data.Datasets
import tensorflow as tf


# We process our datasets elements here (categorical) and convert them to indices to avoid this step
# during model training since only tensorflow support strings.
def process(features, target):
    for feature_name in features:
        if feature_name in CATEGORICAL_FEATURES_WITH_VOCABULARY:
            # Cast categorical feature values to string.
            features[feature_name] = tf.cast(features[feature_name], "string")
            vocabulary = CATEGORICAL_FEATURES_WITH_VOCABULARY[feature_name]
            # Create a lookup to convert a string values to an integer indices.
            # Since we are not using a mask token we leave at default.
            index = layers.StringLookup(
                vocabulary=vocabulary,
                mask_token=None,
                num_oov_indices=1,
                output_mode="int",
            )
            # Convert the string input values into integer indices.
            value_index = index(features[feature_name])
            features[feature_name] = value_index
        else:
            # Do nothing for numerical features
            pass

    # Get the instance weight.
    weight = features.pop(WEIGHT_COLUMN_NAME)
    # Change features from OrderedDict to Dict to match Inputs as they are Dict.
    return dict(features), target, weight


def get_dataset_from_csv(csv_file_path, shuffle=False, batch_size=128):
    dataset = tf.data.experimental.make_csv_dataset(
        csv_file_path,
        batch_size=batch_size,
        column_names=CSV_HEADER,
        column_defaults=COLUMN_DEFAULTS,
        label_name=TARGET_FEATURE_NAME,
        num_epochs=1,
        header=False,
        shuffle=shuffle,
    ).map(process)

    return dataset

"""
## Subclassing Autokeras Graph 

Here we subclass the Autokeras `Graph`

- `build`: we override this method to be able to handle model `Inputs` passed
as dictionaries. In structured data analysis Inputs are normally passed as
dictionaries for each feature of interest

"""

class Graph(ak.graph.Graph):

    def build(self, hp):
        """Build the HyperModel into a Keras Model."""
        keras_nodes = {}
        keras_input_nodes = []
        for node in self.inputs:
            node_id = self._node_to_id[node]
            input_node = node.build_node(hp)
            output_node = node.build(hp, input_node)
            keras_input_nodes.append(input_node)
            keras_nodes[node_id] = output_node
        for block in self.blocks:
            # This code block is to adapt according to Input instance or not
            temp_inputs = None
            for input_node in block.inputs:
                if isinstance(input_node, ak.Input):
                    temp_inputs = temp_inputs if temp_inputs is not None else {}
                    temp_inputs[input_node.name] = keras_nodes[
                        self._node_to_id[input_node]
                    ]
                else:
                    temp_inputs = temp_inputs if temp_inputs is not None else []
                    temp_inputs.append(keras_nodes[self._node_to_id[input_node]])
            outputs = block.build(hp, inputs=temp_inputs)
            outputs = tree.flatten(outputs)
            for output_node, real_output_node in zip(block.outputs, outputs):
                keras_nodes[self._node_to_id[output_node]] = real_output_node
            outputs = block.build(hp, inputs=temp_inputs)
            outputs = tree.flatten(outputs)
            for output_node, real_output_node in zip(block.outputs, outputs):
                keras_nodes[self._node_to_id[output_node]] = real_output_node
        model = keras.models.Model(
            keras_input_nodes,
            [
                keras_nodes[self._node_to_id[output_node]]
                for output_node in self.outputs
            ],
        )
        return self._compile_keras_model(hp, model)

    def _compile_keras_model(self, hp, model):
        # Specify hyperparameters from compile(...)
        optimizer_name = hp.Choice(
            "optimizer",
            ["adam", "sgd"],
            default="adam",
        )
        # TODO: add adadelta optimizer when it can optimize embedding layer on
        # GPU.
        learning_rate = hp.Choice(
            "learning_rate", [1e-1, 1e-2, 1e-3, 1e-4, 2e-5, 1e-5], default=1e-3
        )
        if optimizer_name == "adam":
            optimizer = keras.optimizers.Adam(learning_rate=learning_rate)
        elif optimizer_name == "sgd":
            optimizer = keras.optimizers.SGD(learning_rate=learning_rate)
        model.compile(
            optimizer=optimizer,
            metrics=self._get_metrics(),
            loss=self._get_loss(),
        )

        return model

"""

## Subclassing Autokeras `Input`

Here we subclass the Autokeras Input node object and override the dtype attribute
from None to a user supplied value

"""

class Input(ak.Input):
    def __init__(self, dtype, name=None, **kwargs):
        super().__init__(name=name, **kwargs)
        # Override dtype to a user dtype value
        self.dtype = dtype
        self.name = name

class ClassificationHead(head_module.Head):
    """Classification Dense layers.

    Use sigmoid and binary crossentropy for binary classification and
    multi-label classification. Use softmax and categorical crossentropy for
    multi-class (more than 2) classification. Use Accuracy as metrics by
    default.

    The targets passing to the head would have to be tf.data.Dataset,
    np.ndarray, pd.DataFrame or pd.Series. It can be raw labels, one-hot encoded
    if more than two classes, or binary encoded for binary classification.

    The raw labels will be encoded to one column if two classes were found,
    or one-hot encoded if more than two classes were found.

    # Arguments
        num_classes: Int. Defaults to None. If None, it will be inferred from
            the data.
        multi_label: Boolean. Defaults to False.
        loss: A Keras loss function. Defaults to use `binary_crossentropy` or
            `categorical_crossentropy` based on the number of classes.
        metrics: A list of Keras metrics. Defaults to use 'accuracy'.
        dropout: Float. The dropout rate for the layers.
            If left unspecified, it will be tuned automatically.
    """

    def __init__(
        self,
        num_classes: Optional[int] = None,
        multi_label: bool = False,
        loss: Optional[types.LossType] = None,
        metrics: Optional[types.MetricsType] = None,
        dropout: Optional[float] = None,
        **kwargs
    ):
        self.num_classes = num_classes
        self.multi_label = multi_label
        self.dropout = dropout
        if metrics is None:
            metrics = ["accuracy"]
        if loss is None:
            loss = self.infer_loss()
        super().__init__(loss=loss, metrics=metrics, **kwargs)
        # Infered from analyser.
        self._encoded = None
        self._encoded_for_sigmoid = None
        self._encoded_for_softmax = None
        self._add_one_dimension = False
        self._labels = None
        self.shape = (1,) # MANUFACTURED INTO

    def infer_loss(self):
        if not self.num_classes:
            return None
        if self.num_classes == 2 or self.multi_label:
            return losses.BinaryCrossentropy()
        return losses.CategoricalCrossentropy()

    def get_config(self):
        config = super().get_config()
        config.update(
            {
                "num_classes": self.num_classes,
                "multi_label": self.multi_label,
                "dropout": self.dropout,
            }
        )
        return config

    def build(self, hp, inputs=None):

        # import pdb
        # pdb.set_trace()

        inputs = tree.flatten(inputs)
        utils.validate_num_inputs(inputs, 1)
        input_node = inputs[0]
        output_node = input_node

        # Reduce the tensor to a vector.
        if len(output_node.shape) > 2:
            output_node = reduction.SpatialReduction().build(hp, output_node)

        if self.dropout is not None:
            dropout = self.dropout
        else:
            dropout = hp.Choice("dropout", [0.0, 0.25, 0.5], default=0)

        if dropout > 0:
            output_node = layers.Dropout(dropout)(output_node)
        output_node = layers.Dense(self.shape[-1])(output_node)
        if isinstance(self.loss, keras.losses.BinaryCrossentropy):
            output_node = layers.Activation(
                activations.sigmoid, name=self.name
            )(output_node)
        else:
            output_node = layers.Softmax(name=self.name)(output_node)
        return output_node

# This is a keras layer from the original script
class GatedLinearUnit(layers.Layer):
    def __init__(self, num_units, activation, **kwargs):
        super().__init__(**kwargs)
        self.linear = layers.Dense(num_units)
        self.sigmoid = layers.Dense(num_units, activation=activation)

    def call(self, inputs):
        return self.linear(inputs) * self.sigmoid(inputs)

    def build(self):
        self.built = True

# This is a keras layer from the original script
class GatedResidualNetwork(layers.Layer):

    def __init__(
        self, num_units, dropout_rate, activation, use_layernorm=None, **kwargs
    ):
        super().__init__(**kwargs)
        self.num_units = num_units
        self.use_layernorm = use_layernorm
        self.elu_dense = layers.Dense(num_units, activation=activation)
        self.linear_dense = layers.Dense(num_units)
        self.dropout = layers.Dropout(dropout_rate)
        self.gated_linear_unit = GatedLinearUnit(num_units, activation)
        self.layer_norm = layers.LayerNormalization()
        self.project = layers.Dense(num_units)

    def call(self, inputs, hp):
        x = self.elu_dense(inputs)
        x = self.linear_dense(x)
        x = self.dropout(x)
        if inputs.shape[-1] != self.num_units:
            inputs = self.project(inputs)
        x = inputs + self.gated_linear_unit(x)
        use_layernorm = self.use_layernorm
        if use_layernorm is None:
            use_layernorm = hp.Boolean("use_layernorm", default=True)
        if use_layernorm:
            x = self.layer_norm(x)
        return x

    def build(self):
        self.built = True

"""
## Building the Autokeras `Block`

We have converted the following keras layer to an Autokeras Block to include
hyperapameters to tune. Refer to Autokeras blocks API for writing custom Blocks.

"""

class VariableSelection(ak.Block):
    def __init__(
        self,
        num_units: Optional[Union[int, hyperparameters.Choice]] = None,
        dropout_rate: Optional[Union[float, hyperparameters.Choice]] = None,
        activation: Optional[Union[str, hyperparameters.Choice]] = None,
        **kwargs,
    ):
        super().__init__(**kwargs)
        self.dropout = utils.get_hyperparameter(
            dropout_rate,
            hyperparameters.Choice("dropout", [0.0, 0.25, 0.5], default=0.0),
            float,
        )
        self.num_units = utils.get_hyperparameter(
            num_units,
            hyperparameters.Choice(
                "num_units", [16, 32, 64, 128, 256, 512, 1024], default=16
            ),
            int,
        )
        self.activation = utils.get_hyperparameter(
            activation,
            hyperparameters.Choice(
                "vsn_activation", ["sigmoid", "elu"], default="sigmoid"
            ),
            str,
        )

    def build(self, hp, inputs):
        num_units = utils.add_to_hp(self.num_units, hp, "num_units")
        dropout_rate = utils.add_to_hp(self.dropout, hp, "dropout_rate")
        activation = utils.add_to_hp(self.activation, hp, "activation")
        concat_inputs = []
        for input_ in inputs:
            if input_ in CATEGORICAL_FEATURES_WITH_VOCABULARY:
                vocabulary = CATEGORICAL_FEATURES_WITH_VOCABULARY[input_]
                max_index = len(vocabulary) - 1  # Clamp the indices
                # torch had some index errors during embedding hence the clip function
                embedded_feature = layers.Embedding(
                    input_dim=len(vocabulary),
                    output_dim=num_units,
                )(keras.ops.clip(inputs[input_], 0, max_index))
                concat_inputs.append(embedded_feature)
            else:
                # Project the numeric feature to encoding_size using linear transformation.
                proj_feature = keras.ops.expand_dims(inputs[input_], -1)
                proj_feature = layers.Dense(units=num_units)(proj_feature)
                concat_inputs.append(proj_feature)

        v = layers.concatenate(concat_inputs)
        v = GatedResidualNetwork(
            num_units=num_units, dropout_rate=dropout_rate, activation=activation
        )(v, hp=hp)
        v = keras.ops.expand_dims(
            layers.Dense(units=len(inputs), activation=activation)(v), axis=-1
        )
        x = []
        for idx, input in enumerate(concat_inputs):
            x.append(
                GatedResidualNetwork(
                    num_units=num_units, dropout_rate=dropout_rate, activation=activation
                )(input, hp=hp)
            )
        x = keras.ops.stack(x, axis=1)
        return keras.ops.squeeze(
            keras.ops.matmul(keras.ops.transpose(v, axes=[0, 2, 1]), x), axis=1
        )

# We craete the HyperModel Inputs which will be built into Keras Input objects
def create_model_inputs():
    inputs = {}
    for feature_name in FEATURE_NAMES:
        if feature_name in CATEGORICAL_FEATURES_WITH_VOCABULARY:
            # Make them int64, they are Categorical (whole units)
            inputs[feature_name] = Input(name=feature_name, shape=(), dtype="int64")
        else:
            # Make them float32, they are Real numbers
            inputs[feature_name] = Input(name=feature_name, shape=(), dtype="float32")
    return inputs

"""

## Autokeras `HyperModel`

Here we use the Autokeras `Functional` API to construct a network of BlocksSSS which will
be built into a HyperModel and finally to a Keras Model.

"""

class MyHyperModel(keras_tuner.HyperModel):

    def build(self, hp):
        inputs = create_model_inputs()
        features = VariableSelection()(inputs)
        outputs = ClassificationHead(num_classes=2, multi_label=False)(features)
        model = Graph(inputs=inputs, outputs=outputs)
        model = model.build(hp)
        return model

    def fit(self, hp, model, *args, **kwargs):
        return model.fit(
            *args,
            # Tune whether to shuffle the data in each epoch.
            shuffle=hp.Boolean("shuffle"),
            **kwargs,
        )

"""
## Compile, train, and evaluate the model
"""

print("Start training the model...")
train_dataset = get_dataset_from_csv(
    train_data_file, shuffle=True, batch_size=256
)
valid_dataset = get_dataset_from_csv(valid_data_file, batch_size=256)

import pdb
pdb.set_trace()

tuner = keras_tuner.RandomSearch(
    MyHyperModel(),
    objective="val_accuracy",
    max_trials=3,
    overwrite=True,
    directory="my_dir",
    project_name="tune_hypermodel",
)

tuner.search(train_dataset, epochs=2, validation_data=valid_dataset)

"""
Still exeperencing errors in this section

print("Evaluating model performance...")
test_dataset = get_dataset_from_csv(test_data_file, batch_size=batch_size)
_, accuracy = auto_model.evaluate(test_dataset)
print(f"Test accuracy: {round(accuracy * 100, 2)}%")
"""
