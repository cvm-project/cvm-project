#ifndef PATCHED_THROW_HPP
#define PATCHED_THROW_HPP

namespace trace_exceptions {

void StoreInfoAndThrow(void *ex, void *info, void (*dest)(void *));

}  // namespace trace_exceptions

#endif  // PATCHED_THROW_HPP
