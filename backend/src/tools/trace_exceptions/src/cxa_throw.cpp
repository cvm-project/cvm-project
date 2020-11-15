#include "patched_throw.hpp"

extern "C" {
// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp,-warnings-as-errors)
void __cxa_throw(void *ex, void *info, void (*dest)(void *)) {
    trace_exceptions::StoreInfoAndThrow(ex, info, dest);
}
}
