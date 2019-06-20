#ifndef CPP_PARTITION_OPERATOR_H
#define CPP_PARTITION_OPERATOR_H

#include <list>
#include <type_traits>
#include <vector>

#include "Optional.h"
#include "Utils.h"

template <class Upstream, class Tuple>
class PartitionOperator {
public:
    using InnerArray = decltype(std::declval<Tuple>().v1);
    using InnerTuple = typename std::remove_reference<decltype(
            std::declval<InnerArray>().data[0])>::type;

    static constexpr size_t kBlockCapacity = 4096;

    PartitionOperator(Upstream *const upstream, const size_t num_partitions)
        : upstream_(upstream), num_partitions_(num_partitions) {}

    INLINE void open() {
        upstream_->open();

        partitions_.clear();

        std::vector<InnerArray> current_partition_blocks(num_partitions_);
        for (auto &b : current_partition_blocks) {
            b.data = reinterpret_cast<InnerTuple *>(
                    malloc(sizeof(InnerTuple) * kBlockCapacity));
            b.outer_shape[0] = 0;
            b.shape[0] = 0;
            b.offsets[0] = 0;
        }

        while (auto const ret = upstream_->next()) {
            const auto input_tuple = ret.value();
            const long idx =
                    std::hash<long>()(input_tuple.v0) % num_partitions_;
            auto &current_block = current_partition_blocks[idx];
            if (current_block.outer_shape[0] >= kBlockCapacity) {
                current_block.shape[0] = current_block.outer_shape[0];
                partitions_.push_back(Tuple{idx, current_block});
                current_block.data = reinterpret_cast<InnerTuple *>(
                        malloc(sizeof(InnerTuple) * kBlockCapacity));
                current_block.outer_shape[0] = 0;
                current_block.shape[0] = 0;
                current_block.offsets[0] = 0;
            }
            new (current_block.data + current_block.outer_shape[0]++)
                    InnerTuple(input_tuple);
        }

        for (size_t i = 0; i < current_partition_blocks.size(); i++) {
            auto &current_block = current_partition_blocks[i];
            current_block.shape[0] = current_block.outer_shape[0];
            partitions_.push_back(Tuple{static_cast<long>(i), current_block});
        }

        output_it_ = partitions_.begin();

        upstream_->close();
    }

    INLINE Optional<Tuple> next() {
        if (output_it_ == partitions_.end()) return {};
        return *(output_it_++);
    }

    INLINE void close() {}

private:
    Upstream *const upstream_;
    const size_t num_partitions_;
    std::list<Tuple> partitions_;
    typename std::list<Tuple>::iterator output_it_;
};

template <class Tuple, class Upstream>
PartitionOperator<Upstream, Tuple> makePartitionOperator(
        Upstream *const upstream, const size_t num_partitions) {
    return PartitionOperator<Upstream, Tuple>(upstream, num_partitions);
};

#endif  // CPP_PARTITION_OPERATOR_H
