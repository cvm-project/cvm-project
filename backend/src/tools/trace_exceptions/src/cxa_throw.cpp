#include "patched_throw.hpp"

extern "C" {
void __cxa_throw(void *ex, void *info, void (*dest)(void *)) {
    trace_exceptions::StoreInfoAndThrow(ex, info, dest);
}
}
