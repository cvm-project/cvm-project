from _ast import *
from queue import Queue

from blaze.ast_generator import _find_top_node, ASTGenerator, RDD, ShuffleRDD


def for_(it_var, iterator, body_list):
    return For(target=Name(id=it_var, ctx=Store()), iter=Name(id=iterator, ctx=Load()),
               body=body_list, orelse=[])


def assign_(left, right):
    return Assign(targets=[Name(id=left, ctx=Store())],
                  value=right)


def call_(f_name, f_var):
    return Call(func=Name(id=f_name, ctx=Load()),
                args=[Name(id=f_var, ctx=Load())], keywords=[])


def if_(predicate):
    return If(test=predicate,
              body=[Continue()],
              orelse=[])
