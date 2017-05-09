from numba import njit

from blaze.rdd import *
from blaze.pretty_print import parseprint
from ast import *



def ast_decorator(func):
    def wrapper(self, node):
        func(self, node)
        if node.successor is not None:
            node.successor.accept(self)

    return wrapper


class ASTGenerator:
    def __init__(self):
        self._var_counter = 0
        self._var_prefix = "v"
        self._func_prefix = "f"
        self._func_counter = 0
        self._locals = {}
        self._root_ast = []
        self._ast = self._root_ast

    def _get_next_var(self):
        self._var_counter += 1
        return self._var_prefix + str(self._var_counter)

    def _get_cur_var(self):
        return self._var_prefix + str(self._var_counter)

    def _get_loop_var(self):
        return self._var_prefix + "0"

    def _get_next_func_name(self):
        self._func_counter += 1
        return self._func_prefix + str(self._func_counter)

    def _get_current_func_name(self):
        return self._func_prefix + str(self._func_counter)

    def go(self, node):

        #setup ast
        body_ast = []
        ast = Module(body=[FunctionDef(name='f', args=arguments(args=[arg(arg='values', annotation=None)], vararg=None,
                                                              kwonlyargs=[], kw_defaults=[], kwarg=None, defaults=[]),
                                     body=body_ast, decorator_list=[], returns=None)])

        self._ast = body_ast
        self._root_ast = body_ast
        # climb up to the source
        cur_node = self._find_top_node(node)



        cur_node.accept(self)
        self.add_final_statement_inner()

        # compile and execute ast
        fix_missing_locations(ast)
        parseprint(ast)
        c = compile(ast, "<string>", "exec")
        exec(c, self._locals)
        f = njit(self._locals['f'])
        # with open('/home/sabir/llvm.ll', 'w') as file:
        #     llvm = f.inspect_llvm()
        #     for k, v in llvm.items():
        #         file.write(str(k))
        #         file.write(v)
        #         file.write('\n')

        return f(self._values)

    def add_final_statement_inner(self):
        self._ast.append(Expr(value=Yield(value=Name(id=self._get_cur_var(), ctx=Load()))))

    def add_final_statement_root(self, stm):
        self._root_ast.append(stm)

    def prepend_statement_root(self, stm):
        self._root_ast.insert(0, stm)

    @ast_decorator
    def visit_map(self, node):
        print("visiting a map")
        # add the node function to the locals list
        self._locals[self._get_next_func_name()] = njit(node.func)
        cur_var = self._get_cur_var()
        self._ast.append(
            Assign(targets=[Name(id=self._get_next_var(), ctx=Store())],
                   value=Call(func=Name(id=self._get_current_func_name(), ctx=Load()),
                              args=[Name(id=cur_var, ctx=Load())], keywords=[])))

    @ast_decorator
    def visit_filter(self, node):
        print("visiting a filter")
        # add the node function to the locals list
        self._locals[self._get_next_func_name()] = njit(node.func)
        cur_var = self._get_cur_var()
        self._ast.append(
            If(test=UnaryOp(op=Not(), operand=Call(func=Name(id=self._get_current_func_name(), ctx=Load()),
                                                   args=[Name(id=cur_var, ctx=Load())], keywords=[])),
               body=[Continue()],
               orelse=[]))

    @ast_decorator
    def visit_flat_map(self, node):
        print("visiting a flatMap")
        # add the node function to the locals list
        self._locals[self._get_next_func_name()] = njit(node.func)
        cur_var = self._get_cur_var()
        # produce a new list
        self._ast.append(
            Assign(targets=[Name(id=self._get_next_var(), ctx=Store())],
                   value=Call(func=Name(id=self._get_current_func_name(), ctx=Load()),
                              args=[Name(id=cur_var, ctx=Load())], keywords=[])))
        # add new loop
        body_ast = []
        cur_var = self._get_cur_var()
        self._ast.append(
            For(target=Name(id=self._get_next_var(), ctx=Store()), iter=Name(id=cur_var, ctx=Load()),
                body=body_ast, orelse=[])
        )
        self._ast = body_ast


    @ast_decorator
    def visit_join(self, node):
        pass


    @ast_decorator
    def visit_text_source(self, node):
        path = node.path
        # add a loop over the lines
        # for line in open(path):
        # self._ast.append(
        #         For(target=Name(id=self._get_cur_var(), ctx=Store()), iter=Name(id='values', ctx=Load()),
        #             body=body_ast, orelse=[])
        # )

    @ast_decorator
    def visit_collection_source(self, node):
        values = node.values
        self._values = values
        self._locals['values'] = values
        body_ast = []
        self._ast.append(For(target=Name(id=self._get_cur_var(), ctx=Store()), iter=Name(id='values', ctx=Load()),
                    body=body_ast, orelse=[])
        )
        self._ast = body_ast

    def _find_top_node(self, node):
        cur_node = node
        while cur_node.parent is not None:
            parent = cur_node.parent
            parent.successor = cur_node
            cur_node = parent
        return cur_node