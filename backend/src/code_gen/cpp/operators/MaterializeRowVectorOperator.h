#ifndef CPP_MATERIALIZEROWVECTOROPERATOR_H
#define CPP_MATERIALIZEROWVECTOROPERATOR_H

#include <stdexcept>
#include <type_traits>

#include "Utils.h"
#include "runtime/memory/free_ref_counter.hpp"
#include "runtime/memory/shared_pointer.hpp"
#include "runtime/operators/optional.hpp"

template <typename T, typename U>
auto constexpr max(T x, U y) ->
        typename std::remove_reference<decltype(x > y ? x : y)>::type {
    return x > y ? x : y;
}

template <class OuterTuple, class Upstream>
class MaterializeRowVectorOperator {
    using InnerTuple = std::remove_reference_t<decltype(
            std::declval<OuterTuple>().v0.data[0])>;

public:
    MaterializeRowVectorOperator(Upstream *const upstream)
        : upstream_(upstream), has_returned_(false) {}

    INLINE void open() { has_returned_ = false; }

    INLINE Optional<OuterTuple> next() {
        if (has_returned_) return {};
        has_returned_ = true;

        upstream_->open();

        size_t allocated_size = 1;
        InnerTuple *result_ptr = reinterpret_cast<InnerTuple *>(
                malloc(sizeof(InnerTuple) * allocated_size));
        size_t result_size = 0;

        while (auto tuple = upstream_->next()) {
            if (allocated_size <= result_size) {
                allocated_size *= 2;
                result_ptr = reinterpret_cast<InnerTuple *>(realloc(
                        result_ptr, sizeof(InnerTuple) * allocated_size));
                if (result_ptr == nullptr)
                    throw std::runtime_error(
                            "Could not materialize: out of memory.");
            }
            new (result_ptr + result_size) InnerTuple(tuple.value());
            result_size++;
        }

        result_ptr = reinterpret_cast<InnerTuple *>(realloc(
                result_ptr, sizeof(InnerTuple) * max(size_t(1), result_size)));

        upstream_->close();
        return OuterTuple{
                runtime::memory::SharedPointer<InnerTuple>(
                        new runtime::memory::FreeRefCounter<InnerTuple>(
                                result_ptr, result_size)),
                result_size, 0, result_size};
    }

    INLINE void close() {}

private:
    Upstream *const upstream_;
    bool has_returned_;
};

template <class OuterTuple, class Upstream>
MaterializeRowVectorOperator<OuterTuple, Upstream>
makeMaterializeRowVectorOperator(Upstream *const upstream) {
    return MaterializeRowVectorOperator<OuterTuple, Upstream>(upstream);
};

#endif  // CPP_MATERIALIZEROWVECTOROPERATOR_H
