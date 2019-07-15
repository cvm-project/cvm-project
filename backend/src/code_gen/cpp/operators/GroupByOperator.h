#ifndef CPP_GROUPBY_OPERATOR_H
#define CPP_GROUPBY_OPERATOR_H

#include <unordered_map>
#include <vector>

#include "Utils.h"
#include "runtime/memory/free_ref_counter.hpp"
#include "runtime/memory/shared_pointer.hpp"
#include "runtime/operators/optional.hpp"

template <class Upstream, class Tuple>
class GroupByOperator {
public:
    using OutputArray = decltype(std::declval<Tuple>().v1);
    using InnerTuple = typename std::remove_reference<decltype(
            std::declval<OutputArray>().data[0])>::type;

    GroupByOperator(Upstream *const upstream) : upstream_(upstream) {}

    INLINE void open() {
        if (!result_.empty()) return;

        upstream_->open();

        // Store tuples into a vector per partition
        while (const auto ret = upstream_->next()) {
            auto const input_tuple = ret.value();
            result_[input_tuple.v0].emplace_back(InnerTuple{input_tuple.v1});
        }
        current_output_it_ = result_.begin();

        upstream_->close();
    }

    INLINE Optional<Tuple> next() {
        if (current_output_it_ == result_.end()) return {};

        auto &result_data = current_output_it_->second;

        // Convert the vector of the current partition into an array
        Tuple output;
        output.v0 = current_output_it_->first;
        output.v1.offsets[0] = 0;
        output.v1.outer_shape[0] = result_data.size();
        output.v1.shape[0] = output.v1.outer_shape[0];
        output.v1.data = runtime::memory::SharedPointer<InnerTuple>(
                new runtime::memory::FreeRefCounter<InnerTuple>(
                        malloc(sizeof(InnerTuple) * output.v1.outer_shape[0]),
                        output.v1.outer_shape[0]));
        assert(output.v1.outer_shape[0] == 0 ||
               output.v1.data.get() != nullptr);

        for (size_t i = 0; i < result_data.size(); i++) {
            new (output.v1.data.get() + i)
                    InnerTuple(std::move(result_data[i]));
        }

        current_output_it_++;

        return output;
    }

    INLINE void close() {}

private:
    Upstream *const upstream_;
    std::unordered_map<long, std::vector<InnerTuple>> result_{};
    typename decltype(result_)::iterator current_output_it_{};
};

template <class Tuple, class Upstream>
auto makeGroupByOperator(Upstream *const upstream) {
    return GroupByOperator<Upstream, Tuple>(upstream);
};

#endif  // CPP_GROUPBY_OPERATOR_H
