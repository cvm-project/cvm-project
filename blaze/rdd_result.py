import numpy as np


# empty wrapper over built-in type numpy array
# with this we are able to keep the cffi memory reference
# inside the object in an attribute
class NumpyResult(np.ndarray):
    pass
