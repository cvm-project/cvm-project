import numpy as np


class RDDCollectionResult:
    def __init__(self, vals):
        assert isinstance(vals, np.ndarray)
        self._numpy_array = vals

    def __str__(self):
        return str(self.as_list())

    def as_list(self):
        return self._numpy_array.tolist()

    def as_numpy_array(self):
        return self._numpy_array

    def next(self):
        return self.iter.next()

    def __iter__(self):
        self.iter = self._numpy_array.__iter__()
        return self.iter
