#ifndef CODE_GEN_OPERATORS_PARTITION_OPERATOR_H
#define CODE_GEN_OPERATORS_PARTITION_OPERATOR_H

#include <list>
#include <type_traits>
#include <vector>

#include "Utils.h"
#include "runtime/jit/memory/free_ref_counter.hpp"
#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/operators/murmur_hash2.hpp"
#include "runtime/jit/operators/optional.hpp"

template <class MainUpstream, class ConfUpstream, class Tuple,
          const size_t kSeed>
class PartitionOperator {
public:
    using InnerArray = decltype(std::declval<Tuple>().v1);
    using InnerTuple = typename std::remove_reference<decltype(
            std::declval<InnerArray>().data[0])>::type;

    static constexpr size_t kBlockCapacity = 4096;

    PartitionOperator(MainUpstream *const main_upstream,
                      ConfUpstream *const conf_upstream)
        : main_upstream_(main_upstream), conf_upstream_(conf_upstream) {}

    INLINE void open() {
        // Get fanout from upstream
        conf_upstream_->open();
        fanout_ = conf_upstream_->next().value().v0;
        assert(!conf_upstream_->next());
        conf_upstream_->close();

        // Partition data from main upstream
        main_upstream_->open();

        partitions_.clear();

        std::vector<InnerArray> current_partition_blocks(fanout_);
        for (auto &b : current_partition_blocks) {
            b.data = runtime::memory::SharedPointer<InnerTuple>(
                    new runtime::memory::FreeRefCounter<InnerTuple>(
                            malloc(sizeof(InnerTuple) * kBlockCapacity),
                            kBlockCapacity));
            b.outer_shape[0] = 0;
            b.shape[0] = 0;
            b.offsets[0] = 0;
        }

        while (auto const ret = main_upstream_->next()) {
            const auto input_tuple = ret.value();
            const long idx = runtime::operators::MurmurHash2::Hash(
                                     &input_tuple.v0,
                                     sizeof(decltype(input_tuple.v0)), kSeed) %
                             fanout_;
            auto &current_block = current_partition_blocks[idx];
            if (current_block.outer_shape[0] >= kBlockCapacity) {
                current_block.shape[0] = current_block.outer_shape[0];
                partitions_.push_back(Tuple{idx, current_block});
                current_block.data = runtime::memory::SharedPointer<InnerTuple>(
                        new runtime::memory::FreeRefCounter<InnerTuple>(
                                malloc(sizeof(InnerTuple) * kBlockCapacity),
                                kBlockCapacity));
                current_block.outer_shape[0] = 0;
                current_block.shape[0] = 0;
                current_block.offsets[0] = 0;
            }
            new (current_block.data.get() + current_block.outer_shape[0]++)
                    InnerTuple(input_tuple);
        }

        for (size_t i = 0; i < current_partition_blocks.size(); i++) {
            auto &current_block = current_partition_blocks[i];
            current_block.shape[0] = current_block.outer_shape[0];
            auto const rc =
                    dynamic_cast<runtime::memory::FreeRefCounter<InnerTuple> *>(
                            current_block.data.ref_counter());
            assert(rc != nullptr);
            rc->set_num_elements(current_block.outer_shape[0]);
            partitions_.push_back(Tuple{static_cast<long>(i), current_block});
        }

        output_it_ = partitions_.begin();

        main_upstream_->close();
    }

    INLINE Optional<Tuple> next() {
        if (output_it_ == partitions_.end()) return {};
        return *(output_it_++);
    }

    INLINE void close() {}

private:
    MainUpstream *const main_upstream_;
    ConfUpstream *const conf_upstream_;
    size_t fanout_;
    std::list<Tuple> partitions_;
    typename std::list<Tuple>::iterator output_it_;
};

template <class Tuple, const size_t kSeed, class MainUpstream,
          class ConfUpstream>
PartitionOperator<MainUpstream, ConfUpstream, Tuple, kSeed>
makePartitionOperator(MainUpstream *const main_upstream,
                      ConfUpstream *const conf_upstream) {
    return PartitionOperator<MainUpstream, ConfUpstream, Tuple, kSeed>(
            main_upstream, conf_upstream);
};

#endif  // CODE_GEN_OPERATORS_PARTITION_OPERATOR_H
