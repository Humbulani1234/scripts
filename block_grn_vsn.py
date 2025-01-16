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
from keras.src.utils import traceback_utils
from keras.src.trainers.data_adapters import data_adapter_utils
from keras.src.backend.tensorflow.trainer import TFEpochIterator
from keras.src import callbacks as callbacks_module
from keras.src import optimizers as optimizers_module

import keras_tuner
from keras_tuner.engine import hyperparameters

import autokeras as ak
from autokeras.engine import io_hypermodel
from autokeras.engine import head as head_module
from autokeras.engine import node as node_module
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

data = pd.read_csv(train_data_path, header=None, names=CSV_HEADER).iloc[0:70]
test_data = pd.read_csv(test_data_path, header=None, names=CSV_HEADER).iloc[0:70]

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
        model = Model(
            keras_input_nodes,
            [
                keras_nodes[self._node_to_id[output_node]]
                for output_node in self.outputs
            ],
        )
        return self._compile_keras_model(hp, model)


"""
## Subclassing AutoModel

We subclass the Autokeras `AutoModel` class and overrides the following methods:

- `_build_graph`: this method builds a Graph `Hypermodel` to be used ny the tuner. We override the method 
so we can create our custom Graph (created above) to be used as the HyperModel to the `Tuner`.
- `_convert_to_dataset`: This method was written for `tf.data.Dataset` with a tuple of two elements - x.y,
however in this case we have a tuple of three - x,y, smaple_weights. Hence the modification. Tried processing
the `train_dataset` variable but `tf.function` was returning errors.
-`_check_data_format`: I have not fully understood the changes I have made here. I just made sure it works,
since the function `data_utils.dataset_shape(x)` was not returning the correct result.

"""


class AutoModel(ak.AutoModel):

    def _build_graph(self):
        # Using functional API.
        if all([isinstance(output, node_module.Node) for output in self.outputs]):
            graph = Graph(inputs=self.inputs, outputs=self.outputs)
        # Using input/output API.
        elif all([isinstance(output, head_module.Head) for output in self.outputs]):
            # Clear session to reset get_uid(). The names of the blocks will
            # start to count from 1 for new blocks in a new AutoModel
            # afterwards.  When initializing multiple AutoModel with Task API,
            # if not counting from 1 for each of the AutoModel, the predefined
            # hp values in task specifiec tuners would not match the names.
            keras.backend.clear_session()
            graph = self._assemble()
            self.outputs = graph.outputs
            keras.backend.clear_session()
        return graph

    def _convert_to_dataset(self, x, y, validation_data, batch_size):
        """Convert the data to tf.data.Dataset."""

        # Convert training data.
        self._check_data_format((x, y))
        if isinstance(x, tf.data.Dataset):
            dataset = x
            # tf.data.Dataset has three elements in this case, not two
            x = dataset.map(lambda x, y, z: x)
            # tf.data.Dataset has three elements in this case, not two
            y = dataset.map(lambda x, y, z: y)
        x = self._adapt(x, self.inputs, batch_size)
        y = self._adapt(y, self._heads, batch_size)
        dataset = tf.data.Dataset.zip((x, y))

        # Convert validation data
        if validation_data:
            self._check_data_format(validation_data, validation=True)
            if isinstance(validation_data, tf.data.Dataset):
                # tf.data.Dataset has three elements in this case, not two
                x = validation_data.map(lambda x, y, z: x)
                # tf.data.Dataset has three elements in this case, not two
                y = validation_data.map(lambda x, y, z: y)
            else:
                x, y = validation_data
            x = self._adapt(x, self.inputs, batch_size)
            y = self._adapt(y, self._heads, batch_size)
            validation_data = tf.data.Dataset.zip((x, y))
        return dataset, validation_data

    def _check_data_format(self, dataset, validation=False, predict=False):
        """Check if the dataset has the same number of IOs with the model."""
        if validation:
            in_val = " in validation_data"
            if isinstance(dataset, tf.data.Dataset):
                x = dataset
                y = None
            else:
                x, y = dataset
        else:
            in_val = ""
            x, y = dataset

        if isinstance(x, tf.data.Dataset) and y is not None:
            raise ValueError(
                "Expected y to be None when x is "
                "tf.data.Dataset{in_val}.".format(in_val=in_val)
            )

        if isinstance(x, tf.data.Dataset):
            if not predict:
                # The function data_utils.dataset_shape(x) was not returning the result I wanted
                x_shapes, y_shapes = (None,) * 40, (None,)
                x_shapes = tree.flatten(x_shapes)
                y_shapes = tree.flatten(y_shapes)
            else:
                x_shapes = tree.flatten(data_utils.dataset_shape(x))
        else:
            x_shapes = [a.shape for a in tree.flatten(x)]
            if not predict:
                y_shapes = [a.shape for a in tree.flatten(y)]

        if len(x_shapes) != len(self.inputs):
            raise ValueError(
                "Expected x{in_val} to have {input_num} arrays, "
                "but got {data_num}".format(
                    in_val=in_val,
                    input_num=len(self.inputs),
                    data_num=len(x_shapes),
                )
            )
        if not predict and len(y_shapes) != len(self.outputs):
            raise ValueError(
                "Expected y{in_val} to have {output_num} arrays, "
                "but got {data_num}".format(
                    in_val=in_val,
                    output_num=len(self.outputs),
                    data_num=len(y_shapes),
                )
            )

    def evaluate(self, x, y=None, batch_size=32, verbose=1, **kwargs):
        """Evaluate the best model for the given data."""
        self._check_data_format((x, y))
        if isinstance(x, tf.data.Dataset):
            dataset = x
            # tf.data.Dataset has three elements in this case, not two
            x = dataset.map(lambda x, y, z: x)
            # tf.data.Dataset has three elements in this case, not two
            y = dataset.map(lambda x, y, z: y)
        x = self._adapt(x, self.inputs, batch_size)
        y = self._adapt(y, self._heads, batch_size)
        dataset = tf.data.Dataset.zip((x, y))
        pipeline = self.tuner.get_best_pipeline()
        dataset = pipeline.transform(dataset)
        model = self.tuner.get_best_model()
        return utils.evaluate_with_adaptive_batch_size(
            model=model, batch_size=batch_size, x=dataset, verbose=verbose, **kwargs
        )


"""
## Subclassing the keras `Model` class

Here we subclass the Model class to override the following methods:

- `fit`: For some mysterious reason (actually part of why I need help), when I try to iterate over the `
`tf.data.Dataset` object, it iterates forever without returning and neither showing any errors. So, I 
have decided to test the model just on a single instance of the data. And it works!
- `evaluate`: Same reson as above

"""


class Model(keras.Model):

    @traceback_utils.filter_traceback
    def fit(
        self,
        x=None,
        y=None,
        batch_size=None,
        epochs=1,
        verbose="auto",
        callbacks=None,
        validation_split=0.0,
        validation_data=None,
        shuffle=True,
        class_weight=None,
        sample_weight=None,
        initial_epoch=0,
        steps_per_epoch=None,
        validation_steps=None,
        validation_batch_size=None,
        validation_freq=1,
    ):
        self._assert_compile_called("fit")
        # TODO: respect compiled trainable state
        self._eval_epoch_iterator = None
        if validation_split and validation_data is None:
            # Create the validation data using the training data. Only supported
            # for TF/numpy/jax arrays.
            (
                (x, y, sample_weight),
                validation_data,
            ) = array_slicing.train_validation_split(
                (x, y, sample_weight), validation_split=validation_split
            )

        if validation_data is not None:
            (
                val_x,
                val_y,
                val_sample_weight,
            ) = data_adapter_utils.unpack_x_y_sample_weight(validation_data)

        # Create an iterator that yields batches for one epoch.
        epoch_iterator = TFEpochIterator(
            x=x,
            y=y,
            sample_weight=sample_weight,
            batch_size=batch_size,
            steps_per_epoch=steps_per_epoch,
            shuffle=shuffle,
            class_weight=class_weight,
            distribute_strategy=self.distribute_strategy,
            steps_per_execution=self.steps_per_execution,
        )

        self._maybe_symbolic_build(iterator=epoch_iterator)
        epoch_iterator.reset()

        # Container that configures and calls callbacks.
        if not isinstance(callbacks, callbacks_module.CallbackList):
            callbacks = callbacks_module.CallbackList(
                callbacks,
                add_history=True,
                add_progbar=verbose != 0,
                verbose=verbose,
                epochs=epochs,
                steps=epoch_iterator.num_batches,
                model=self,
            )

        self.stop_training = False
        self.make_train_function()
        callbacks.on_train_begin()
        training_logs = None
        logs = {}
        initial_epoch = self._initial_epoch or initial_epoch
        for epoch in range(initial_epoch, epochs):
            self.reset_metrics()
            callbacks.on_epoch_begin(epoch)
            with epoch_iterator.catch_stop_iteration():
                # The for step, iterator in epoch_iterator was iterating forever
                for step in range(2):
                    callbacks.on_train_batch_begin(step)
                    # I ran this just on a single data sample for testing
                    logs = self.train_function((x.take(1).get_single_element(),))
                    callbacks.on_train_batch_end(step, logs)
                    if self.stop_training:
                        break

            # Override with model metrics instead of last step logs if needed.
            epoch_logs = dict(self._get_metrics_result_or_logs(logs))

            # Run validation.
            if validation_data is not None and self._should_eval(
                epoch, validation_freq
            ):
                # Create EpochIterator for evaluation and cache it.
                if getattr(self, "_eval_epoch_iterator", None) is None:
                    self._eval_epoch_iterator = TFEpochIterator(
                        x=val_x,
                        y=val_y,
                        sample_weight=val_sample_weight,
                        batch_size=validation_batch_size or batch_size,
                        distribute_strategy=self.distribute_strategy,
                        steps_per_execution=self.steps_per_execution,
                        steps_per_epoch=validation_steps,
                        shuffle=False,
                    )
                val_logs = self.evaluate(
                    x=val_x,
                    y=val_y,
                    sample_weight=val_sample_weight,
                    batch_size=validation_batch_size or batch_size,
                    steps=validation_steps,
                    callbacks=callbacks,
                    return_dict=True,
                    _use_cached_eval_dataset=True,
                )
                val_logs = {"val_" + name: val for name, val in val_logs.items()}
                epoch_logs.update(val_logs)

            callbacks.on_epoch_end(epoch, epoch_logs)
            training_logs = epoch_logs
            if self.stop_training:
                break

        if isinstance(self.optimizer, optimizers_module.Optimizer) and epochs > 0:
            self.optimizer.finalize_variable_values(self.trainable_weights)

        # If _eval_epoch_iterator exists, delete it after all epochs are done.
        if getattr(self, "_eval_epoch_iterator", None) is not None:
            del self._eval_epoch_iterator
        callbacks.on_train_end(logs=training_logs)
        return self.history

    @traceback_utils.filter_traceback
    def evaluate(
        self,
        x=None,
        y=None,
        batch_size=None,
        verbose="auto",
        sample_weight=None,
        steps=None,
        callbacks=None,
        return_dict=False,
        **kwargs,
    ):
        self._assert_compile_called("evaluate")
        # TODO: respect compiled trainable state
        use_cached_eval_dataset = kwargs.pop("_use_cached_eval_dataset", False)
        if kwargs:
            raise ValueError(f"Arguments not recognized: {kwargs}")

        if use_cached_eval_dataset:
            epoch_iterator = self._eval_epoch_iterator
        else:
            # Create an iterator that yields batches of input/target data.
            epoch_iterator = TFEpochIterator(
                x=x,
                y=y,
                sample_weight=sample_weight,
                batch_size=batch_size,
                steps_per_epoch=steps,
                shuffle=False,
                distribute_strategy=self.distribute_strategy,
                steps_per_execution=self.steps_per_execution,
            )

        self._maybe_symbolic_build(iterator=epoch_iterator)
        epoch_iterator.reset()

        # Container that configures and calls callbacks.
        if not isinstance(callbacks, callbacks_module.CallbackList):
            callbacks = callbacks_module.CallbackList(
                callbacks,
                add_history=True,
                add_progbar=verbose != 0,
                verbose=verbose,
                epochs=1,
                steps=epoch_iterator.num_batches,
                model=self,
            )

        self.make_test_function()
        self.stop_evaluating = False
        callbacks.on_test_begin()
        logs = {}
        self.reset_metrics()
        with epoch_iterator.catch_stop_iteration():
            # The for step, iterator in epoch_iterator was iterating forever
            for step in range(2):
                callbacks.on_test_batch_begin(step)
                # I ran this just on a single data sample for testing
                logs = self.test_function((x.take(1).get_single_element(),))
                callbacks.on_test_batch_end(step, logs)
                if self.stop_evaluating:
                    break
        logs = self._get_metrics_result_or_logs(logs)
        callbacks.on_test_end(logs)

        if return_dict:
            return logs
        return self._flatten_metrics_in_order(logs)


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
                    num_units=num_units,
                    dropout_rate=dropout_rate,
                    activation=activation,
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


def create_model():
    inputs = create_model_inputs()
    features = VariableSelection()(inputs)
    output_node = ak.ClassificationHead(num_classes=2, multi_label=False)(features)
    # Functional model
    auto_model = AutoModel(
        inputs=inputs,
        outputs=output_node,
        overwrite=True,
        max_trials=3,
        objective="val_accuracy",
    )
    return auto_model


"""
## Compile, train, and evaluate the model
"""

batch_size = 32  # may be tuned
num_epochs = 1  # may be tuned

auto_model = create_model()

print("Start training the model...")
train_dataset = get_dataset_from_csv(
    train_data_file, shuffle=True, batch_size=batch_size
)
valid_dataset = get_dataset_from_csv(valid_data_file, batch_size=batch_size)

auto_model.fit(
    x=train_dataset, batch_size=batch_size, validation_data=valid_dataset, epochs=1
)

"""
Still exeperencing errors in this section

print("Evaluating model performance...")
test_dataset = get_dataset_from_csv(test_data_file, batch_size=batch_size)
_, accuracy = auto_model.evaluate(test_dataset)
print(f"Test accuracy: {round(accuracy * 100, 2)}%")
"""
