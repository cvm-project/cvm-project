from queue import Queue

from blaze.ast_generator import _find_top_node, ASTGenerator, RDD, ShuffleRDD


class StageAst:

    def __init__(self):
        self._var_counter = 0
        self._var_prefix = "v"
        self._func_prefix = "f"
        self._func_counter = 0
        self.locals = {}
        self.root_ast = []
        self.inner_ast = self.root_ast
        self.func_def = None

    def get_next_var(self):
        self._var_counter += 1
        return self._var_prefix + str(self._var_counter)

    def get_cur_var(self):
        return self._var_prefix + str(self._var_counter)

    def get_loop_var(self):
        return self._var_prefix + "0"

    def get_next_func_name(self):
        self._func_counter += 1
        return self._func_prefix + str(self._func_counter)

    def get_current_func_name(self):
        return self._func_prefix + str(self._func_counter)