from queue import Queue

from blaze.ast_generator import _find_top_node, ASTGenerator, RDD, ShuffleRDD, Join
from blaze.stage_ast import StageAst


class Stage:
    def __init__(self, stage_ast=None):
        self.stage_ast = stage_ast
        self.sink_stage = None
        self.source_stages = []

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


def is_stage_breaker(rdd):
    return isinstance(rdd, ShuffleRDD)


def append_breaker_ast(source_breaker, cur_stage):
    if isinstance(source_breaker, Join):

        if source_breaker.hash_right:
            hash_stage_ast = cur_stage.source_stages[0].stage_ast
            other_stage_ast = cur_stage.source_stages[1].stage_ast
        else:
            hash_stage_ast = cur_stage.source_stages[1].stage_ast
            other_stage_ast = cur_stage.source_stages[0].stage_ast

        # TODO: hashing by the key


class StageGenerator:
    def schedule(self, rdd):
        """create a DAG of stages"""

        def go(sink_stage, sink_node):
            source_breaker = sink_node
            cur_stage = sink_stage
            if is_stage_breaker(sink_node):
                for parent in sink_node.parents:
                    go(sink_stage, parent)
            else:
                cur_stage, source_breaker = self.create_stage(sink_node)
                cur_stage.connect_sink(sink_stage)
                for parent in source_breaker.parents:
                    go(cur_stage, parent)

            # add breaker specific code
            append_breaker_ast(source_breaker, cur_stage)
        go(None, rdd)

    @staticmethod
    def create_stage(bot_node):
        # create a stage down to the bot_node, inclusive
        cur_node = bot_node
        stop = False
        breaker_node = None
        while True:
            for parent in cur_node.parents:
                if is_stage_breaker(parent):
                    breaker_node = parent
                    stop = True
            if stop or not cur_node.parents:
                break

            assert len(cur_node.parents) == 1
            cur_node = cur_node.parents[0]
        stage_ast = StageAst()
        ASTGenerator(stage_ast).generate_ast(breaker_node.successor, bot_node)
        return Stage(stage_ast), breaker_node
