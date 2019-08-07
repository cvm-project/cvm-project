// This library is a work-around that enables nice function names in LLVM
// sanitizer output.
//
// The problem is that the sanitizers look up function names when the program
// is exiting, at which point dynamically loaded functions have already been
// closed and, hence, functions names are not available anymore. This library
// implements a noop closing mechanism, which effectively disables closing. If
// loaded with DL_PRELOAD, this makes nice function names work.
//
// Taken from
// https://github.com/google/sanitizers/issues/89#issuecomment-406316683

#include <stdio.h>

int dlclose(void *handle) { return 0; }
