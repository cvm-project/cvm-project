# file "example_build.py"

# Note: we instantiate the same 'cffi.FFI' class as in the previous
# example, but call the result 'ffibuilder' now instead of 'ffi';
# this is to avoid confusion with the other 'ffi' object you get below

from cffi import FFI
ffibuilder = FFI()
c_path = '../c_operators/'
headers = [c_path + 'intersect.h', c_path + 'add.h']

ffibuilder.set_source("operators",
   r""" // passed to the real C compiler
        #include <sys/types.h>
        #include "../c_operators/intersect.c"
        #include "../c_operators/add.c"
    """,
    libraries=[])


for header in headers:
    with open(header, 'r') as c_def:
        ffibuilder.cdef(c_def.read())

if __name__ == "__main__":
    ffibuilder.compile(verbose=True)