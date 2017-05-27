import ast
from ast import fix_missing_locations

from numba import jit
from numba import njit
from blaze.pretty_print import parseprint
import astunparse

from ast import *

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
        self.stage = None
        self.input_values = None

    def execute(self, left, right=None):

        # parseprint(self.func_def)
        print(astunparse.unparse(self.func_def))
        fix_missing_locations(self.func_def)
        # print(ast.dump(self.func_def))
        c = compile(self.func_def, "<string>", "exec")
        exec(c, self.locals)
        f = self.locals['f']
        return f(left, right)

    def get_next_var(self):
        self._var_counter += 1
        return self._var_prefix + str(self._var_counter)

    def get_current_var(self):
        return self._var_prefix + str(self._var_counter)

    def get_loop_var(self):
        return self._var_prefix + "loop"

    def get_next_func_name(self):
        self._func_counter += 1
        return self._func_prefix + str(self._func_counter)

    def get_current_func_name(self):
        return self._func_prefix + str(self._func_counter)

    def append_inner_ast(self, ast):
        self.inner_ast.append(ast)

    def append_root_ast(self, ast):
        self.root_ast.append(ast)
