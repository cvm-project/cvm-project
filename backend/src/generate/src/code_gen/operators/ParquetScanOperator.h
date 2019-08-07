#ifndef CPP_PARQUETSCANOPERATOR_H
#define CPP_PARQUETSCANOPERATOR_H

#include <memory>

#include <cstdlib>

#include "Utils.h"
#include "runtime/operators/optional.hpp"
#include "runtime/operators/parquet_scan.hpp"
#include "runtime/values/array.hpp"

template <class OutputTuple, class Upstream, class... OutputTypes>
class ParquetScanOperator {
    using IndexSequence = std::make_index_sequence<sizeof...(OutputTypes)>;
    using OutputStdTuple = std::tuple<OutputTypes...>;

    class FileNameFromUpstreamOperator
        : public runtime::operators::FileNameOperator {
    public:
        explicit FileNameFromUpstreamOperator(Upstream* const upstream)
            : upstream_(upstream) {}

        void open() override { upstream_->open(); }

        Optional<std::string> next() override {
            auto const input = upstream_->next();
            if (!input) return {};
            return input.value().v0;
        }

        void close() override { upstream_->close(); }

    private:
        Upstream* const upstream_;
    };

public:
    ParquetScanOperator(
            Upstream* const upstream,
            std::vector<std::vector<
                    std::shared_ptr<runtime::operators::Predicate>>>&&
                    range_predicates,
            std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
            const std::string& filesystem)
        : upstream_(runtime::operators::MakeParquetScanOperator(
                  std::make_unique<FileNameFromUpstreamOperator>(upstream),
                  std::move(range_predicates), std::move(column_types),
                  std::move(col_ids), filesystem)) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<OutputTuple> next() {
        const auto input = upstream_->next();
        return ValueToTuple<OutputTuple>(input);
    }

    INLINE void close() { upstream_->close(); }

private:
    std::unique_ptr<runtime::operators::ParquetScanOperator> upstream_;
};

template <class OutputTuple, class Upstream, class... OutputTypes>
ParquetScanOperator<OutputTuple, Upstream, OutputTypes...>
makeParquetScanOperator(
        Upstream* const upstream,
        std::vector<
                std::vector<std::shared_ptr<runtime::operators::Predicate>>>&&
                range_predicates,
        std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
        const std::string& filesystem,
        const std::tuple<OutputTypes...>& /*tag*/) {
    return ParquetScanOperator<OutputTuple, Upstream, OutputTypes...>(
            upstream, std::move(range_predicates), std::move(column_types),
            std::move(col_ids), filesystem);
};

template <class OutputTuple, class Upstream>
auto makeParquetScanOperator(
        Upstream* const upstream,
        std::vector<
                std::vector<std::shared_ptr<runtime::operators::Predicate>>>&&
                range_predicates,
        std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
        const std::string& filesystem) {
    using StdOutputTuple =
            decltype(TupleToStdTuple(std::declval<OutputTuple>()));
    return makeParquetScanOperator<OutputTuple, Upstream>(
            upstream, std::move(range_predicates), std::move(column_types),
            std::move(col_ids), filesystem, StdOutputTuple());
}

#endif  // CPP_PARQUETSCANOPERATOR_H
