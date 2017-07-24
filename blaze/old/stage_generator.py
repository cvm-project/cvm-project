from blaze.ast_generator import ROOT_HASH_TABLE, RESULT_LIST, ASTGenerator, FUNCTION_NAME, PARAMETER_LIST, \
    PARAMETER_HASH_TABLE
from blaze.stage_ast import StageAst
from blaze.ast_shortcuts import *
import blaze.rdd


class Stage:
    def __init__(self, stage_ast=None):
        if stage_ast is not None:
            stage_ast.stage = self
        self.stage_ast = stage_ast
        self.sink_stage = None
        self.source_stages = []
        self.hash_right = True
        self.is_empty = False

    def connect_sink(self, sink_stage):
        if sink_stage is None:
            return
        self.sink_stage = sink_stage
        sink_stage.source_stages.append(self)

    def connect_source(self, source_stage):
        if source_stage is None:
            return
        self.source_stages.append(source_stage)
        source_stage.sink_stage = self

    def compute(self):
        if self.is_empty:
            return self.source_stages[0].compute()
        if self.stage_ast.input_values:
            # use the input
            if len(self.source_stages) > 0:
                hashtable = self.source_stages[0].compute()
                return self.stage_ast.execute(self.stage_ast.input_values, hashtable)
            else:
                return self.stage_ast.execute(self.stage_ast.input_values)
        else:
            # go to the sources
            if len(self.source_stages) > 1:
                # join
                # one of them is the hash table
                if self.hash_right:
                    _list = self.source_stages[0].compute()
                    hash_table = self.source_stages[1].compute()
                else:
                    hash_table = self.source_stages[0].compute()
                    _list = self.source_stages[1].compute()

                return self.stage_ast.execute(_list, hash_table)
            else:
                return self.stage_ast.execute(self.source_stages[0].compute())


def is_stage_breaker(rdd):
    return isinstance(rdd, blaze.rdd.ShuffleRDD)


def append_breaker_ast(source_breaker, cur_stage):
    """
    append build code to the source stages asts
    :param source_breaker:
    :param cur_stage:
    :return:
    """
    if isinstance(source_breaker, blaze.rdd.Join):

        # mark the sources as not empty
        for source in cur_stage.source_stages:
            source.is_empty = False

        if source_breaker.hash_right:
            hash_stage_ast = cur_stage.source_stages[1].stage_ast
            other_stage_ast = cur_stage.source_stages[0].stage_ast
            other_stage = cur_stage.source_stages[0]
            hash_stage = cur_stage.source_stages[1]
        else:
            cur_stage.hash_right = False
            hash_stage_ast = cur_stage.source_stages[0].stage_ast
            other_stage_ast = cur_stage.source_stages[1].stage_ast
            other_stage = cur_stage.source_stages[1]
            hash_stage = cur_stage.source_stages[0]

        # could otherwise use a UDF to determine the key
        # hash_table[var_name[0]]=var_name
        hash_stage_ast.append_inner_ast(
            assign_explicit(left=subscript_store(var_name=ROOT_HASH_TABLE, slice=index_(value=
            subscript_load(
                var_name=hash_stage_ast.get_current_var(),
                slice=index_num(0)
            ))),
                    right=name_load(name=hash_stage_ast.get_current_var())))

        # return hash_table
        hash_stage_ast.append_root_ast(
            return_(ROOT_HASH_TABLE)
        )

        cur_tuple = other_stage_ast.get_current_var()
        other_tuple = other_stage_ast.get_next_var()

        # other_tuple = param_hash_table.get(tuple_name[0], None)
        other_stage_ast.append_inner_ast(Assign(targets=[Name(id=other_tuple, ctx=Store())], value=Call(
            func=Attribute(value=Name(id=PARAMETER_HASH_TABLE, ctx=Load()), attr='get', ctx=Load()),
            args=[Subscript(value=Name(id=cur_tuple, ctx=Load()), slice=Index(value=Num(n=0)), ctx=Load()),
                  NameConstant(value=None)], keywords=[])))
        # if other_tuple is None:
        #   continue
        other_stage_ast.append_inner_ast(If(
            test=Compare(left=Name(id=other_tuple, ctx=Load()), ops=[Is()], comparators=[NameConstant(value=None)]),
            body=[Continue()], orelse=[]))

        # else join and return the list
        # joined = cur + other[1:]

        other_stage_ast.append_inner_ast(Assign(targets=[Name(id=other_stage_ast.get_next_var(), ctx=Store())],
                                                value=BinOp(left=Name(id=cur_tuple, ctx=Load()), op=Add(),
                                                            right=Subscript(value=Name(id=other_tuple, ctx=Load()),
                                                                            slice=Slice(lower=Num(n=1), upper=None,
                                                                                        step=None),
                                                                            ctx=Load()))))
        # root_list.append(joined)
        other_stage_ast.append_inner_ast(Expr(
            value=Call(func=Attribute(value=Name(id=RESULT_LIST, ctx=Load()), attr='append', ctx=Load()),
                       args=[Name(id=other_stage_ast.get_current_var(), ctx=Load())], keywords=[])))
        other_stage_ast.append_root_ast(return_(RESULT_LIST))

        #relink the stages
        other_stage.connect_source(hash_stage)
        cur_stage.source_stages.remove(hash_stage)


def generate_breaker_stage(node):
    """
    generate empty stage for case where there are consecutive breakers
    empty stages should be removed afterwards
    :param node:
    :return:
    """
    if isinstance(node, blaze.rdd.Join):
        stage_ast = StageAst()
        body_ast = []

        stage_ast.func_def = Module(
            body=[FunctionDef(name=FUNCTION_NAME, args=arguments(
                args=[arg(arg=PARAMETER_LIST, annotation=None), arg(arg=PARAMETER_HASH_TABLE, annotation=None)],
                vararg=None,
                kwonlyargs=[], kw_defaults=[], kwarg=None, defaults=[NameConstant(value=None)]),
                              body=body_ast, decorator_list=[], returns=None)])

        # always add a hash_table
        body_ast.append(assign_(ROOT_HASH_TABLE, Dict(keys=[], values=[])))
        body_ast.append(assign_(RESULT_LIST, List(elts=[], ctx=Load())))

        stage_ast.root_ast = body_ast
        body_ast = []
        stage_ast.root_ast.append(
            For(target=Name(id=stage_ast.get_current_var(), ctx=Store()), iter=Name(id=PARAMETER_LIST, ctx=Load()),
                body=body_ast, orelse=[]))

        cur_var = stage_ast.get_current_var()
        body_ast.append(
            assign_(left=stage_ast.get_next_var(), right=name_load(cur_var))
        )
        stage_ast.inner_ast = body_ast

        stage = Stage(stage_ast)
        stage.is_empty = True
        return stage


def schedule(rdd):
    """
    generate a DAG of stages
    :param rdd:
    :return: the last sink stage
    """

    def go(sink_stage, sink_node):
        if is_stage_breaker(sink_node):
            breaker_stage = generate_breaker_stage(sink_node)
            breaker_stage.connect_sink(sink_stage)
            for parent in sink_node.parents:
                go(breaker_stage, parent)
            append_breaker_ast(sink_node, breaker_stage)
            return breaker_stage
        else:
            mid_stage, source_breaker = create_stage(sink_node)
            if is_stage_breaker(source_breaker):
                breaker_stage = generate_breaker_stage(source_breaker)
                breaker_stage.connect_sink(mid_stage)
                mid_stage.connect_sink(sink_stage)
                for parent in source_breaker.parents:
                    go(breaker_stage, parent)
                append_breaker_ast(source_breaker, breaker_stage)
                return mid_stage
            else:
                mid_stage.connect_sink(sink_stage)
                for parent in source_breaker.parents:
                    go(mid_stage, parent)
                return mid_stage

    # add the last return statement if the stage is not empty
    sink_stage = go(None, rdd)
    if sink_stage.is_empty:
        return sink_stage.source_stages[0]
    else:
        # sink_stage.stage_ast.append_inner_ast(Expr(
        #     value=Call(func=Attribute(value=Name(id=RESULT_LIST, ctx=Load()), attr='append', ctx=Load()),
        #                args=[Name(id=sink_stage.stage_ast.get_current_var(), ctx=Load())], keywords=[])))
        sink_stage.stage_ast.append_root_ast(return_(RESULT_LIST))
        return sink_stage


def create_stage(sink_node):
    # create a stage down to the bot_node, inclusive
    cur_node = sink_node
    stop = False
    breaker_node = None
    while True:
        for parent in cur_node.parents:
            breaker_node = parent
            if is_stage_breaker(parent):
                breaker_node = parent
                stop = True
        if stop or not cur_node.parents:
            if breaker_node is None:
                breaker_node = cur_node
            break

        assert len(cur_node.parents) == 1
        cur_node = cur_node.parents[0]
    stage_ast = StageAst()
    if is_stage_breaker(breaker_node):
        ASTGenerator(stage_ast, breaker_node.successor, sink_node).generate_ast()
    else:
        ASTGenerator(stage_ast, breaker_node, sink_node).generate_ast()
    return Stage(stage_ast), breaker_node
