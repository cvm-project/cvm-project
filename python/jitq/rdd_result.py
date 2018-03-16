import numpy as np


def astuple(tuplelike):
    try:
        return tuple(astuple(i) for i in tuplelike)
    except TypeError:
        return tuplelike


def tuples(items):
    return (astuple(t) for t in items)


# empty wrapper over built-in type numpy array
# with this we are able to keep the cffi memory reference
# inside the object in an attribute
class NumpyResult(np.ndarray):
    def astuples(self):
        return tuples(self)

    def astuplelist(self):
        return list(self.astuples())
