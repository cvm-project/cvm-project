from _ast import *


def for_(it_var, iterator, body_list):
    return For(target=Name(id=it_var, ctx=Store()), iter=Name(id=iterator, ctx=Load()),
               body=body_list,
               orelse=[])


def assign_(left, right):
    return Assign(targets=[Name(id=left, ctx=Store())],
                  value=right)


def assign_explicit(left, right):
    return Assign(targets=[left],
                  value=right)


def call_(f_name, f_var):
    return Call(func=Name(id=f_name, ctx=Load()),
                args=[Name(id=f_var, ctx=Load())], keywords=[])


def if_(predicate):
    return If(test=predicate,
              body=[Continue()],
              orelse=[])


def subscript_store(var_name, slice):
    return Subscript(value=Name(id=var_name, ctx=Load()), slice=slice, ctx=Store())


def subscript_load(var_name, slice):
    return Subscript(value=Name(id=var_name, ctx=Load()), slice=slice, ctx=Load())


def index_(value):
    return Index(value=value)


def index_num(number):
    return Index(value=Num(n=number))


def name_load(name):
    return Name(id=name, ctx=Load())


def name_store(name):
    return Name(id=name, ctx=Store())


def return_(name):
    return Return(value=Name(id=name, ctx=Load()))
