import ast
import inspect
from types import FunctionType

OPT_UNROLL = "unroll"
OPT_CONST_PROPAGATE = "const_propagate"


class AstOptimizer(ast.NodeTransformer):
    def __init__(self, ns, opts, max_depth=1000):
        self.opts = opts
        self._namespace = ns
        self.max_depth = max_depth

    def visit_Name(self, node):
        if OPT_CONST_PROPAGATE in self.opts:
            self.generic_visit(node)
            value = self._namespace.get(node.id, None)
            if value:
                return ast.Num(value)
        return node

    def visit_For(self, node):
        self.generic_visit(node)
        if OPT_UNROLL in self.opts:
            # only unloop with range iterator
            if isinstance(node.iter, ast.Call) and node.iter.func.id == "range":
                instrs = []
                trg = node.target
                start = 0
                step = 1
                if len(node.iter.args) > 1:
                    try:
                        # give up if any of the numbers weren't evaluated
                        start = node.iter.args[0].n
                        end = node.iter.args[1].n
                    except AttributeError:
                        return node
                    try:
                        step = node.iter.args[2]
                        try:
                            step = step.n
                        except AttributeError:
                            return node
                    except IndexError:
                        pass
                else:
                    try:
                        end = node.iter.args[0].n
                    except AttributeError:
                        return node
                if end < self.max_depth:
                    for i in range(start, end, step):
                        # assign the iter variable
                        instrs.append(ast.Assign(targets=[ast.Name(id=trg.id, ctx=ast.Store())], value=ast.Num(n=i)))
                        # repeat the body
                        instrs += node.body
                    return instrs
        return node


def ast_optimize(func, opts):
    vars = inspect.getclosurevars(func)[0].copy()
    vars.update(inspect.getclosurevars(func)[1])

    src = inspect.getsource(func)
    # remove idents
    lines = src.split("\n")
    tabs = 0
    for c in lines[0]:
        if c == "\t" or c == " ":
            tabs += 1
        else:
            break

    src = "\n".join(map(lambda line: line[tabs:], lines))
    tree = AstOptimizer(vars, opts).visit(ast.parse(src))
    code = compile(ast.fix_missing_locations(tree), filename=inspect.getfile(func), mode="exec")
    func_code = code.co_consts[0]
    callble = FunctionType(func_code, func.__globals__, name=func.__name__,
                           argdefs=func.__defaults__)
    return callble
