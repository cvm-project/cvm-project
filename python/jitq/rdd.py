# pylint: disable=too-many-lines
import abc
import dis
import io
import json
from urllib.parse import urlparse

from cffi import FFI
import numba
from numba import typeof, types
import numpy as np
from pandas import DataFrame

from jitq import c_executor
from jitq.rdd_result import NumpyResult
from jitq.ast_optimizer import OPT_CONST_PROPAGATE, ast_optimize
from jitq.config import FAST_MATH, DUMP_DAG
from jitq.libs.numba.llvm_ir import get_llvm_ir
from jitq.utils import replace_unituple, get_project_path, RDDEncoder, \
    make_tuple, item_typeof, numba_type_to_dtype, is_item_type, C_TYPE_MAP, \
    make_flat_tuple, make_record


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
    # pylint: disable=too-many-public-methods
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
            op_dict['output_type'] = make_flat_tuple(operator.output_type)
            op_dicts[str(operator)] = op_dict

        visitor = RDD.Visitor(collect_operators)
        visitor.visit(self)

        return op_dicts

    def get_final_dict(self):
        inputs = self.get_inputs()
        hash_ = str(hash(self))
        dag_dict, output_type = self.context.serialization_cache.get(
            hash_, (None, None))

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
            output_type = self.output_type

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
        return c_executor.execute(
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

    def map(self, map_func, names=None):
        return Map(self.context, self, map_func, names)

    def alias(self, names):
        return self.map(lambda x: x, names)

    def filter(self, predicate):
        return Filter(self.context, self, predicate)

    def flat_map(self, func):
        return FlatMap(self.context, self, func)

    def reduce_by_key(self, func):
        return ReduceByKey(self.context, self, func)

    def reduce_by_index(self, func, min_idx, max_idx):
        return ReduceByIndex(self.context, self, func, min_idx, max_idx)

    def reduce(self, func):
        return EnsureSingleTuple(self.context,
                                 Reduce(self.context, self, func)) \
            .execute_dag()

    def join(self, other):
        return Join(self.context, self, other)

    def antijoin(self, other):
        return AntiJoin(self.context, self, other)

    def cartesian(self, other):
        return Cartesian(self.context, self, other)

    def topk(self, num_elements):
        return TopK(self.context, self, num_elements)

    def sort(self):
        return Sort(self.context, self)

    def zip(self, other):
        return Zip(self.context, self, other)

    def collect(self):
        return MaterializeRowVector(self.context, self).execute_dag()

    def to_parquet(self, filename, column_names, conf=None):
        conf = conf or {}
        conf['filename'] = filename
        conf['filesystem'] = 'file'
        try:
            url = urlparse(filename)
            if url.scheme == 's3':
                conf['filesystem'] = 's3'
        except BaseException:
            pass
        conf_value = {
            'type': 'tuple',
            'fields': [{'type': 'std::string', 'value': json.dumps(conf)}],
        }
        parents = [MaterializeColumnVector(self.context, self),
                   ParameterLookup(self.context,
                                   make_tuple([types.unicode_type]),
                                   conf_value)]
        return \
            EnsureSingleTuple(
                self.context,
                MaterializeParquetFile(
                    self.context, parents, column_names)) \
            .execute_dag()

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

    def __init__(self, context, parent):
        super(EnsureSingleTuple, self).__init__(context, parent)
        self.output_type = self.parents[0].output_type

    def self_write_dag(self, dic):
        pass


class Map(PipeRDD):
    NAME = 'map'

    def __init__(self, context, parent, func, names):
        super(Map, self).__init__(context, parent, func)
        self.llvm_ir, self.output_type = get_llvm_ir_and_output_type(
            self.func, [self.parents[0].output_type])
        if names is not None:
            self.output_type = make_record(self.output_type, names)
        if not is_item_type(self.output_type):
            raise BaseException(
                "Function given to map has the wrong return type:\n"
                "  found:    {0}".format(self.output_type))

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir


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
            self.output_type = replace_unituple(types.Array(dtype, 1, "C"))


class MaterializeColumnVector(UnaryRDD):
    NAME = 'materialize_column_chunks'

    def __init__(self, context, parent):
        super(MaterializeColumnVector, self).__init__(context, parent)

        input_type = self.parents[0].output_type

        # Construct output type
        column_types = []
        for type_ in input_type.types:
            column_types.append(types.Array(type_, 1, "C"))
        self.output_type = make_tuple(column_types)

    def self_write_dag(self, dic):
        pass


class MaterializeParquetFile(BinaryRDD):
    NAME = 'materialize_parquet_file'

    def __init__(self, context, parents, column_names):
        super(MaterializeParquetFile, self).__init__(context, parents)

        self.column_names = column_names
        self.output_type = types.unicode_type

    def self_write_dag(self, dic):
        dic['column_names'] = self.column_names

    def self_hash(self):
        hash_objects = [
            '##'.join(self.column_names),
        ]
        return hash("#".join(hash_objects))


class Filter(PipeRDD):
    NAME = 'filter'

    def __init__(self, context, parent, func):
        super(Filter, self).__init__(context, parent, func)
        self.llvm_ir, return_type = get_llvm_ir_and_output_type(
            self.func, [self.parents[0].output_type])
        if str(return_type) != "bool":
            raise BaseException(
                "Function given to filter has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format("bool", return_type))
        self.output_type = self.parents[0].output_type

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir


class FlatMap(PipeRDD):
    NAME = 'flat_map'

    def __init__(self, context, parent, func):
        super(FlatMap, self).__init__(context, parent, func)
        self.llvm_ir, self.output_type = get_llvm_ir_for_generator(self.func)

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir


class Join(BinaryRDD):
    NAME = 'join'

    """
    the first element in a tuple is the key
    """

    def __init__(self, context, left, right):
        super(Join, self).__init__(context, [left, right])
        self.output_type = self.compute_output_type()

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
        pass


class TopK(UnaryRDD):
    NAME = 'topk'

    def __init__(self, context, parent, num_elements):
        super(TopK, self).__init__(context, parent)
        self.output_type = self.parents[0].output_type
        self.num_elements = num_elements
        if self.num_elements <= 0:
            raise TypeError(
                "TopK takes a positive parameter.\n"
                "  found :    {0}\n"
                .format(self.num_elements))

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.num_elements)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        dic['num_elements'] = self.num_elements


class Sort(UnaryRDD):
    NAME = 'sort'

    def __init__(self, context, parent):
        super(Sort, self).__init__(context, parent)
        self.output_type = self.parents[0].output_type

    def self_hash(self):
        hash_objects = [str(self.output_type)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        pass


class AntiJoin(BinaryRDD):
    NAME = 'antijoin'

    """
    the first element in a tuple is the key
    """

    def __init__(self, context, left, right):
        super(AntiJoin, self).__init__(context, [left, right])
        self.output_type = self.compute_output_type()

    def compute_output_type(self):
        left_type = self.parents[0].output_type
        right_type = self.parents[1].output_type
        if not isinstance(left_type, types.Tuple):
            left_type = make_tuple([left_type])
        if not isinstance(right_type, types.Tuple):
            right_type = make_tuple([right_type])

        if str(left_type[0]) != str(right_type[0]):
            raise TypeError(
                "AntiJoin keys must be of matching type.\n"
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

        return make_tuple((key_type,) + left_payload)

    def self_write_dag(self, dic):
        pass


class Cartesian(BinaryRDD):
    NAME = 'cartesian'

    def __init__(self, context, left, right):
        super(Cartesian, self).__init__(context, [left, right])
        self.output_type = self.compute_output_type()

    def compute_output_type(self):
        left_type = self.parents[0].output_type
        right_type = self.parents[1].output_type
        if not isinstance(left_type, types.Tuple):
            left_type = make_tuple([left_type])
        if not isinstance(right_type, types.Tuple):
            right_type = make_tuple([right_type])
        return make_tuple(left_type.types + right_type.types)

    def self_write_dag(self, dic):
        pass


class Zip(BinaryRDD):
    NAME = 'zip'

    def __init__(self, context, *args):
        super(Zip, self).__init__(context, list(args))
        self.output_type = self.compute_output_type()

    def compute_output_type(self):
        output_types = []
        for parent in self.parents:
            tuple_type = parent.output_type
            if not isinstance(tuple_type, types.Tuple):
                tuple_type = make_tuple([tuple_type])
            for inner_type in tuple_type.types:
                output_types.append(inner_type)
        return make_tuple(tuple(output_types))

    def self_write_dag(self, dic):
        dic['num_inputs'] = len(self.parents)


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
        aggregate_type = self.parents[0].output_type
        self.llvm_ir, self.output_type = get_llvm_ir_and_output_type(
            self.func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(self.output_type):
            raise BaseException(
                "Function given to reduce has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(aggregate_type, self.output_type))

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir


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
        aggregate_type = make_tuple(self.parents[0].output_type.types[1:])
        if len(aggregate_type) == 1:
            aggregate_type = aggregate_type.types[0]
        self.llvm_ir, output_type = get_llvm_ir_and_output_type(
            func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(output_type):
            raise BaseException(
                "Function given to reduce_by_key has the wrong return type:\n"
                "  expected: {0}\n"
                "  found:    {1}".format(aggregate_type, output_type))
        self.output_type = self.parents[0].output_type

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir


class ReduceByIndex(UnaryRDD):
    NAME = 'reduce_by_index'

    """
    binary function must be commutative and associative
    the return value type should be the same as its arguments minus the key
    the input cannot be empty
    """

    def __init__(self, context, parent, func, min_idx, max_idx):
        super(ReduceByIndex, self).__init__(context, parent)
        self.func = func
        self.min_idx = min_idx
        self.max_idx = max_idx
        aggregate_type = make_tuple(self.parents[0].output_type.types[1:])
        if len(aggregate_type) == 1:
            aggregate_type = aggregate_type.types[0]
        self.llvm_ir, output_type = get_llvm_ir_and_output_type(
            func, [aggregate_type, aggregate_type])
        if str(aggregate_type) != str(output_type):
            raise BaseException(
                "Function given to reduce_by_index has the wrong return type:"
                "\n  expected: {0}"
                "\n  found:    {1}".format(aggregate_type, output_type))
        self.output_type = self.parents[0].output_type

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        # Add some extra spice to the hashed value
        file_.write("{}#{}".format(self.min_idx, self.max_idx))
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir
        dic['min'] = self.min_idx
        dic['max'] = self.max_idx


class CSVSource(SourceRDD):
    NAME = 'csv_source'

    def __init__(self, context, path, delimiter=",",
                 dtype=None, add_index=False):
        super(CSVSource, self).__init__(context)
        self.path = path
        self.dtype = dtype
        self.add_index = add_index
        self.delimiter = delimiter
        df = np.genfromtxt(
            self.path, dtype=self.dtype, delimiter=self.delimiter, max_rows=1)
        self.output_type = item_typeof(df[0])

    def self_hash(self):
        hash_objects = [
            self.path,
            str(self.dtype),
            str(self.add_index), self.delimiter,
            self.output_type
        ]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
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

    def __init__(self, context, output_type, value, data=None):
        super(ParameterLookup, self).__init__(context)

        self.output_type = output_type
        self.input_value = value
        self.data = data  # Handle to prevent GC if needed

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
    def make_parent_from_values(context, values):
        # pylint: disable=len-as-condition
        # values could also be a numpy array
        assert len(values) > 0, "Empty collection not allowed"
        assert isinstance(values, DataFrame), \
            "ColumnScan only supports DataFrames currently."

        # Construct output type
        field_types = []
        field_names = []
        for name, dtype in values.dtypes.iteritems():
            field_types.append(types.Array(numba.from_dtype(dtype), 1, "C"))
            field_names.append(name)
        output_type = make_tuple(field_types)

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
                'ref_counter': 0,
                'outer_shape': [num_elements],
                'offsets': [0],
                'shape': [num_elements],
            })
        input_value = {
            'type': 'tuple',
            'fields': column_values,
        }

        parent = ParameterLookup(context, output_type, input_value, values)

        return (parent, field_names)

    def __init__(self, context, parent, add_index, column_names=None):
        super(ColumnScan, self).__init__(context, parent)

        self.add_index = add_index

        field_types = []
        for field_type in parent.output_type.types:
            assert isinstance(field_type, types.Array)
            field_types.append(field_type.dtype)

        # Add names if provided
        if column_names:
            assert len(field_types) == len(column_names)
            field_dtypes = [numba_type_to_dtype(t) for t in field_types]
            fields = list(zip(column_names, field_dtypes))
            self.output_type = numba.from_dtype(np.dtype(fields))
        else:
            self.output_type = make_tuple(field_types)

        self.output_type = replace_unituple(self.output_type)

        # Update output type with field for index, if added
        if add_index:
            self.output_type = _compute_item_type_with_index(self.output_type)

    def self_hash(self):
        hash_objects = [str(self.output_type), str(self.add_index)]
        return hash("#".join(hash_objects))

    def self_write_dag(self, dic):
        dic['add_index'] = self.add_index


class RowScan(UnaryRDD):
    NAME = 'row_scan'

    @staticmethod
    def make_parent_from_values(context, values):
        # pylint: disable=len-as-condition
        # values could also be a numpy array
        assert len(values) > 0, "Empty collection not allowed"

        # Convert values to Numpy array
        item_type = None
        if not isinstance(values, np.ndarray):
            # Special treatment of general iterables: Save the item type
            # here, as a conversion to dtype and back turns Python tuples
            # into Numpy records, which we don't want.
            item_type = item_typeof(values[0])
            values = np.array(values, dtype=numba_type_to_dtype(item_type))

        if values.dtype.isalignedstruct:
            raise NotImplementedError

        assert isinstance(values, np.ndarray)

        # Determine parameter output type
        if item_type is None:
            item_type = item_typeof(values[0])
        output_type = replace_unituple(types.Array(item_type, 1, "C"))

        # Construct input value
        ffi = FFI()
        data = values.__array_interface__['data'][0]
        data = int(ffi.cast("uintptr_t", ffi.cast("void*", data)))
        outer_shape = values.shape

        ref_counter = 0
        if isinstance(values, NumpyResult):
            jitq_tuple = json.loads(values.handle.string)
            ref_counter = jitq_tuple[0]['fields'][0]['ref_counter']

        input_value = {
            'type': 'tuple',
            'fields': [{'type': 'array',
                        'data': data,
                        'ref_counter': ref_counter,
                        'outer_shape': outer_shape,
                        'offsets': [0] * len(outer_shape),
                        'shape': outer_shape}],
        }

        parent = ParameterLookup(context, output_type, input_value, values)

        return parent

    def __init__(self, context, parent, add_index):
        # Construct parameter lookup as parent
        super(RowScan, self).__init__(context, parent)

        assert isinstance(parent.output_type, types.Array)
        self.output_type = parent.output_type.dtype
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
        self.llvm_ir, self.output_type = get_llvm_ir_for_generator(self.func)

    def self_hash(self):
        file_ = io.StringIO()
        dis.dis(self.func, file=file_)
        return hash(file_.getvalue())

    def self_write_dag(self, dic):
        dic['func'] = self.llvm_ir


class ExpandPattern(BinaryRDD):
    NAME = 'expand_pattern'

    def __init__(self, context, filename_or_pattern, pattern_range):
        input_value = {
            'type': 'tuple',
            'fields': [{'type': 'std::string', 'value': filename_or_pattern}],
        }
        parent1 = ParameterLookup(context, types.string, input_value)
        parent2 = Range(context, pattern_range[0], pattern_range[1], 1)
        super(ExpandPattern, self).__init__(context, [parent1, parent2])
        self.output_type = types.string

    def self_write_dag(self, dic):
        dic["output_type"] = self.output_type


class ParquetScan(UnaryRDD):
    NAME = 'parquet_scan'

    def __init__(self, context, parent, columns, filesystem):
        super(ParquetScan, self).__init__(context, parent)
        self.columns = []
        column_types = []
        for col in columns:
            ranges = []
            if len(col) == 3:
                ranges = [{'lo': str(lo), 'hi': str(hi)} for lo, hi in col[2]]
            self.columns.append({'idx': col[0], 'ranges': ranges})
            column_types.append(types.Array(col[1], 1, "C"))
        self.output_type = make_tuple(column_types)
        self.filesystem = filesystem

    def self_hash(self):
        hash_values = [str(self.columns), self.filesystem]
        return hash("#".join(hash_values))

    def self_write_dag(self, dic):
        dic['columns'] = self.columns
        dic['filesystem'] = self.filesystem
