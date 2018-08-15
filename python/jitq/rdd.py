import abc
import dis
import json

import io
import numba
from numba import typeof, types
import numpy as np
from pandas import DataFrame
from cffi import FFI

from jitq.ast_optimizer import OPT_CONST_PROPAGATE, ast_optimize
from jitq.c_executor import ExecutorManager
from jitq.config import FAST_MATH, DUMP_DAG
from jitq.libs.numba.llvm_ir import get_llvm_ir
from jitq.utils import replace_unituple, get_project_path, RDDEncoder, \
    make_tuple, item_typeof, numba_type_to_dtype, is_item_type, C_TYPE_MAP


def clean_rdds(rdd):
    rdd.dic = None
    for par in rdd.parents:
        clean_rdds(par)


def get_llvm_ir_and_output_type(func, arg_types=None, opts=None):
    opts_ = {OPT_CONST_PROPAGATE}
    if opts is not None:
        opts_ += opts
    if arg_types is None:
        arg_types = []
    func = ast_optimize(func, opts_)

    dec_func = numba.njit(
        tuple(arg_types), fastmath=FAST_MATH, parallel=True)(func)

    output_type = dec_func.nopython_signatures[0].return_type
    output_type = replace_unituple(output_type)

    input_type = dec_func.nopython_signatures[0].args

    code = get_llvm_ir(output_type(*input_type), func, fastmath=FAST_MATH)

    return code, output_type


def get_llvm_ir_for_generator(func):
    dec_func = numba.njit(())(func)
    output_type = dec_func.nopython_signatures[0].return_type
    llvm = dec_func.inspect_llvm()[()]
    output_type = output_type.yield_type
    return llvm, output_type


class RDD(abc.ABC):
    NAME = 'abstract'

    """
    Dataset representation.
    RDDs represent dataflow operators.

    """

    class Visitor:
        def __init__(self, func):
            self.func = func
            self.visited = set()

        def visit(self, operator):
            if str(operator) in self.visited:
                return
            self.visited.add(str(operator))
            for parent in operator.parents:
                self.visit(parent)
            self.func(operator)

    def __init__(self, context, parents):
        self.dic = None
        self._cache = False
        self.parents = parents
        self.context = context
        self.output_type = None
        self.hash = None
        if any([p.context != self.context for p in self.parents]):
            raise ValueError("The context of all parents must be the same!")

    def cache(self):
        self._cache = True
        return self

    def write_dag(self):
        op_dicts = dict()

        def collect_operators(operator):
            op_dict = {}
            operator.self_write_dag(op_dict)
            assert is_item_type(operator.output_type), \
                "Expected valid nested tuple type."

            op_dict['id'] = len(op_dicts)
            op_dict['predecessors'] = [
                op_dicts[str(p)]['id'] for p in operator.parents]
            op_dict['op'] = operator.NAME
            op_dict['output_type'] = operator.output_type
            op_dicts[str(operator)] = op_dict

        visitor = RDD.Visitor(collect_operators)
        visitor.visit(self)

        return op_dicts

    def get_final_dict(self):
        inputs = self.get_inputs()
        hash_ = str(hash(self))
        dag_dict, output_type = self.context.serialization_cache.get(
            hash_, None)

        if not dag_dict:
            dag_dict = dict()

            clean_rdds(self)

            operators = self.write_dag()
            dag_dict['operators'] = sorted(operators.values(),
                                           key=lambda d: d['id'])
            dag_dict['outputs'] = [{
                'op': operators[str(self)]['id'],
                'port': 0,
            }]
            dag_dict['inputs'] = [{
                'op': operators[str(op)]['id'],
                'op_port': 0,
                'dag_port': n,
            } for (op, (n, _)) in sorted(inputs.items(),
                                         key=lambda e: e[1][0])]

            self.context.serialization_cache[hash_] = (dag_dict, output_type)
            # write to file
            if DUMP_DAG:
                with open(get_project_path() + '/dag.json', 'w') as fp:
                    json.dump(dag_dict, fp=fp, cls=RDDEncoder, indent=3)
        self.output_type = output_type
        return dag_dict

    def execute_dag(self):
        inputs = self.get_inputs()
        dag_dict = self.get_final_dict()

        input_values = [v for (_, v) in
                        sorted(list(inputs.values()),
                               key=lambda input_: input_[0])]
        return ExecutorManager().execute(
            self.context, dag_dict, input_values, self.output_type)

    def __hash__(self):
        hashes = []

        def collect_hashes(operator):
            op_hash = str(operator.self_hash())
            operator.hash = hash(type(operator).__name__ + op_hash)
            hashes.append(str(operator.hash))

        visitor = RDD.Visitor(collect_hashes)
        visitor.visit(self)
        return hash('#'.join(hashes))

    # pylint: disable=no-self-use
    def self_hash(self):
        return hash("")

    def get_inputs(self):
        inputs = {}

        def collect_inputs(operator):
            op_inputs = operator.self_get_inputs()
            if op_inputs is None:
                return
            inputs[operator] = (len(inputs), op_inputs)

        visitor = RDD.Visitor(collect_inputs)
        visitor.visit(self)
        return inputs

    @abc.abstractmethod
    def self_write_dag(self, dic):
        pass

    def self_get_inputs(self):
        pass

    def map(self, map_func):
        return Map(self.context, self, map_func)

    def filter(self, predicate):
        return Filter(self.context, self, predicate)

    def flat_map(self, func):
        return FlatMap(self.context, self, func)

    def reduce_by_key(self, func):
        return ReduceByKey(self.context, self, func)

    def reduce(self, func):
        return EnsureSingleTuple(self.context,
                                 Reduce(self.context, self, func)) \
            .execute_dag()

    def join(self, other):
        return Join(self.context, self, other)

    def cartesian(self, other):
        return Cartesian(self.context, self, other)

    def collect(self):
        return MaterializeRowVector(self.context, self).execute_dag()

    def count(self):
        ret = self.map(lambda t: 1).reduce(lambda t1, t2: t1 + t2)
        return ret if ret is not None else 0


class SourceRDD(RDD):
    def __init__(self, context):
        super(SourceRDD, self).__init__(context, [])

    @abc.abstractmethod
    def self_write_dag(self, dic):
        pass


class UnaryRDD(RDD):
    def __init__(self, context, parent):
        super(UnaryRDD, self).__init__(context, [parent])

    @abc.abstractmethod
    def self_write_dag(self, dic):
        pass


class BinaryRDD(RDD):
    def __init__(self, context, parents):
        super(BinaryRDD, self).__init__(context, parents)
        assert len(parents) == 2

    @abc.abstractmethod
    def self_write_dag(self, dic):
        pass


class NaryRDD(RDD):

    @abc.abstractmethod
    def self_write_dag(self, dic):
        pass


class PipeRDD(UnaryRDD):
    def __init__(self, context, parent, func):
        super(PipeRDD, self).__init__(context, parent)
        self.func = func

    @abc.abstractmethod
    def self_write_dag(self, dic):
        pass

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())


class EnsureSingleTuple(UnaryRDD):
    NAME = 'ensure_single_tuple'

    def self_write_dag(self, dic):
        self.output_type = self.parents[0].output_type


class Map(PipeRDD):
    NAME = 'map'

    def self_write_dag(self, dic):
        dic['func'], self.output_type = get_llvm_ir_and_output_type(
            self.func, [self.parents[0].output_type])
        if not is_item_type(self.output_type):
            raise BaseException(
                "Function given to map has the wrong return type:\n"
                "  found:    {0}".format(self.output_type))


class MaterializeRowVector(UnaryRDD):
    NAME = 'materialize_row_vector'

    def __init__(self, context, parent):
        # XXX: This is necessary because MaterializeRowVectors are constructed
        #      on the fly for every call to collect and for a cached RDD,
        #      self_write_dag is not executed.
        super(MaterializeRowVector, self).__init__(context, parent)
        self.__compute_output_type()

    def self_write_dag(self, dic):
        self.__compute_output_type()

    def __compute_output_type(self):
        dtype = self.parents[0].output_type
        if isinstance(dtype, (types.Array, types.List)):
            # sabir(25.06.18) the output should be a "jagged" array
            # we cannot guarantee that all sub-arrays will be of the same size
            self.output_type = types.List(dtype)
        else:
            self.output_type = types.Array(dtype, 1, "C")


class Filter(PipeRDD):
    NAME = 'filter'

    def self_write_dag(self, dic):
        dic['func'], return_type = get_llvm_ir_and_output_type(
            self.func, [self.parents[0].output_type])
        if str(return_type) != "bool":
            raise BaseException(
                "Function given to filter has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format("bool", return_type))
        self.output_type = self.parents[0].output_type


class FlatMap(PipeRDD):
    NAME = 'flat_map'

    def self_write_dag(self, dic):
        dic['func'], self.output_type = get_llvm_ir_for_generator(self.func)


class Join(BinaryRDD):
    NAME = 'join'

    """
    the first element in a tuple is the key
    """

    def __init__(self, context, left, right):
        super(Join, self).__init__(context, [left, right])

    def compute_output_type(self):
        left_type = self.parents[0].output_type
        right_type = self.parents[1].output_type
        if not isinstance(left_type, types.Tuple):
            left_type = make_tuple([left_type])
        if not isinstance(right_type, types.Tuple):
            right_type = make_tuple([right_type])

        if str(left_type[0]) != str(right_type[0]):
            raise TypeError(
                "Join keys must be of matching type.\n"
                "  found left:    {0}\n"
                "  found right:   {1}"
                .format(left_type[0], right_type[0]))

        # Special case: two scalar inputs produce a scalar output
        if not isinstance(self.parents[0].output_type, types.Tuple) and \
                not isinstance(self.parents[1].output_type, types.Tuple):
            return self.parents[0].output_type

        # Common case: concatenate tuples
        key_type = left_type[0]
        left_payload = left_type.types[1:]
        right_payload = right_type.types[1:]

        return make_tuple((key_type,) + left_payload + right_payload)

    def self_write_dag(self, dic):
        self.output_type = self.compute_output_type()


class Cartesian(BinaryRDD):
    NAME = 'cartesian'

    def __init__(self, context, left, right):
        super(Cartesian, self).__init__(context, [left, right])

    def compute_output_type(self):
        left_type = self.parents[0].output_type
        right_type = self.parents[1].output_type
        if not isinstance(left_type, types.Tuple):
            left_type = make_tuple([left_type])
        if not isinstance(right_type, types.Tuple):
            right_type = make_tuple([right_type])
        return make_tuple(left_type.types + right_type.types)

    def self_write_dag(self, dic):
        self.output_type = self.compute_output_type()


class Reduce(UnaryRDD):
    NAME = 'reduce'

    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments
    the input cannot be empty
    """

    def __init__(self, context, parent, func):
        super(Reduce, self).__init__(context, parent)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        aggregate_type = self.parents[0].output_type
        dic['func'], self.output_type = get_llvm_ir_and_output_type(
            self.func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(self.output_type):
            raise BaseException(
                "Function given to reduce has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(aggregate_type, self.output_type))


class ReduceByKey(UnaryRDD):
    NAME = 'reduce_by_key'

    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments minus the key
    the input cannot be empty
    """

    def __init__(self, context, parent, func):
        super(ReduceByKey, self).__init__(context, parent)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        aggregate_type = make_tuple(self.parents[0].output_type.types[1:])
        if len(aggregate_type) == 1:
            aggregate_type = aggregate_type.types[0]
        dic['func'], output_type = get_llvm_ir_and_output_type(
            self.func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(output_type):
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(aggregate_type, output_type))
        self.output_type = self.parents[0].output_type


class CSVSource(SourceRDD):
    NAME = 'csv_source'

    def __init__(self, context, path, delimiter=",",
                 dtype=None, add_index=False):
        super(CSVSource, self).__init__(context)
        self.path = path
        self.dtype = dtype
        self.add_index = add_index
        self.delimiter = delimiter

    def self_hash(self):
        hash_objects = [
            self.path,
            str(self.dtype),
            str(self.add_index), self.delimiter,
            self.output_type
        ]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        df = np.genfromtxt(
            self.path, dtype=self.dtype, delimiter=self.delimiter, max_rows=1)
        self.output_type = item_typeof(df[0])
        dic['data_path'] = self.path
        dic['add_index'] = self.add_index


class ConstantTuple(SourceRDD):
    NAME = 'constant_tuple'

    def __init__(self, context, values):
        super(ConstantTuple, self).__init__(context)
        self.values = [str(v) for v in values]
        self.output_type = item_typeof(values)

    def self_hash(self):
        return hash("#".join(self.values))

    def self_write_dag(self, dic):
        dic['values'] = self.values


class ParameterLookup(SourceRDD):
    NAME = 'parameter_lookup'

    def __init__(self, context, output_type, value):
        super(ParameterLookup, self).__init__(context)

        self.output_type = output_type
        self.input_value = value

    def self_hash(self):
        hash_objects = [str(self.output_type)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        pass

    def self_get_inputs(self):
        return self.input_value


# pylint: disable=inconsistent-return-statements
def _compute_item_type(outer_type):
    if isinstance(outer_type, types.Array):
        ndim = outer_type.ndim
        if ndim > 1:
            return types.Array(dtype=outer_type.dtype, ndim=ndim - 1,
                               layout=outer_type.layout)
        return outer_type.dtype
    if isinstance(outer_type, types.List):
        return outer_type.dtype
    assert False, "Cannot have any other containers"


def _compute_item_type_with_index(item_type):
    assert is_item_type(item_type)

    if isinstance(item_type, types.Tuple):
        child_types = item_type.types
        return make_tuple((typeof(0),) + child_types)

    if isinstance(item_type, types.Record):
        fields = item_type.dtype.descr
        # Compute unique key name by extending
        # the longest existing field name
        longest_key = max(fields, key=lambda f: len(f[0]))[0]
        dtypes = [(longest_key + "0", "i8")] + fields
        return numba.from_dtype(np.dtype(dtypes))

    # Atomic
    return make_tuple((typeof(0),) + (item_type,))


class ColumnScan(UnaryRDD):
    NAME = 'column_scan'

    @staticmethod
    def _make_parent(context, values):
        # Construct output type
        fields = []
        for name, dtype in values.dtypes.iteritems():
            fields.append(types.Array(numba.from_dtype(dtype), 1, "C"))
        output_type = make_tuple(fields)

        # Construct input parameter
        column_values = []
        ffi = FFI()
        num_elements = len(values)
        for name, dtype in values.dtypes.iteritems():
            data = values[name].data
            data = int(ffi.cast("uintptr_t", ffi.from_buffer(data)))
            column_values.append({
                'type': 'array',
                'data': data,
                'outer_shape': [num_elements],
                'offsets': [0],
                'shape': [num_elements],
            })
        input_value = {
            'type': 'tuple',
            'fields': column_values,
        }

        return ParameterLookup(context, output_type, input_value)

    def __init__(self, context, values, add_index):
        # pylint: disable=len-as-condition
        # values could also be a numpy array
        assert len(values) > 0, "Empty collection not allowed"
        assert isinstance(values, DataFrame), \
            "ColumnScan only supports DataFrames currently."

        # Construct parameter lookup as parent
        super(ColumnScan, self).__init__(
            context, self._make_parent(context, values))

        # Store data frame to prevent GC
        self.data = values

        # Store operator info
        fields = [(name, dtype) for name, dtype in values.dtypes.iteritems()]
        self.output_type = numba.from_dtype(np.dtype(fields))
        self.add_index = add_index

        # Update output type with field for index, if added
        if add_index:
            self.output_type = _compute_item_type_with_index(self.output_type)

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.add_index)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        dic['add_index'] = self.add_index


class CollectionSource(UnaryRDD):
    """
    accepts any python collections, numpy arrays or pandas dataframes

    passed python collection will be copied into a numpy array
    a reference to the array is stored in this instance
    to prevent freeing the input memory before the end of computation
    """

    NAME = 'row_scan'

    @staticmethod
    def _make_parent(context, values):
        assert isinstance(values, np.ndarray)

        # Determine parameter output type
        item_type = item_typeof(values[0])
        output_type = types.Array(item_type, 1, "C")

        # Construct input parameter
        ffi = FFI()
        data = values.__array_interface__['data'][0]
        data = int(ffi.cast("uintptr_t", ffi.cast("void*", data)))
        outer_shape = values.shape
        input_value = {
            'type': 'tuple',
            'fields': [{'type': 'array',
                        'data': data,
                        'outer_shape': outer_shape,
                        'offsets': [0] * len(outer_shape),
                        'shape': outer_shape}],
        }

        return ParameterLookup(context, output_type, input_value)

    def __init__(self, context, values, add_index):
        # pylint: disable=len-as-condition
        # values could also be a numpy array
        assert len(values) > 0, "Empty collection not allowed"

        # Convert values to Numpy array
        output_type = None
        if not isinstance(values, np.ndarray):
            # Special treatment of general iterables: safe output type
            # here, as a conversion to dtype and back turns Python tuples
            # into Numpy records, which we don't want.
            output_type = item_typeof(values[0])
            values = np.array(values, dtype=numba_type_to_dtype(output_type))

        if values.dtype.isalignedstruct:
            raise NotImplementedError

        # Construct parameter lookup as parent
        super(CollectionSource, self).__init__(
            context, self._make_parent(context, values))

        # Store data frame to prevent GC
        self.data = values

        # Store operator info
        self.output_type = output_type
        if self.output_type is None:
            self.output_type = _compute_item_type(self.parents[0].output_type)
        self.add_index = add_index

        # Update output type with field for index, if added
        if add_index:
            self.output_type = _compute_item_type_with_index(self.output_type)

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.add_index)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        dic['add_index'] = self.add_index


class Range(UnaryRDD):
    NAME = 'range_source'

    def __init__(self, context, from_, to, step):
        values = (from_, to, step)
        value = {
            'type': 'tuple',
            'fields': [{'type': C_TYPE_MAP[str(item_typeof(v))],
                        'value': v} for v in values],
        }
        parent_output_type = item_typeof(values)

        parent = ParameterLookup(context, parent_output_type, value)

        super(Range, self).__init__(context, parent)
        self.output_type = parent.output_type[0]

    def self_write_dag(self, dic):
        pass


class GeneratorSource(SourceRDD):
    NAME = 'generator_source'

    def __init__(self, context, func):
        super(GeneratorSource, self).__init__(context)
        self.func = func

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        dic['func'], self.output_type = get_llvm_ir_for_generator(self.func)
