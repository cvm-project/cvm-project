#ifndef CODE_GEN_OPERATORS_PARQUETSCANOPERATOR_H
#define CODE_GEN_OPERATORS_PARQUETSCANOPERATOR_H

#include <cstdlib>

#include <memory>

#include "TupleToValueOperator.h"
#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/parquet_scan.hpp"
#include "runtime/jit/values/array.hpp"

template <class OutputTuple, class Upstream, class... OutputTypes>
class ParquetScanOperator {
    using IndexSequence = std::make_index_sequence<sizeof...(OutputTypes)>;
    using OutputStdTuple = std::tuple<OutputTypes...>;

public:
    ParquetScanOperator(
            Upstream* const upstream,
            std::vector<std::vector<
                    std::shared_ptr<runtime::operators::Predicate>>>&&
                    range_predicates,
            std::vector<std::string>&& column_types, std::vector<int>&& col_ids,
            const std::string& filesystem)
        : upstream_(runtime::operators::MakeParquetScanOperator(
                  std::make_unique<TupleToValueOperator<Upstream>>(upstream),
                  std::move(range_predicates), std::move(column_types),
                  std::move(col_ids), filesystem)) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<OutputTuple> next() {
        const auto input = upstream_->next();
        return ValueToTuple<OutputTuple>(input);
    }

    INLINE void close() { upstream_->close(); }

private:
    std::unique_ptr<runtime::operators::ValueOperator> upstream_;
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

#endif  // CODE_GEN_OPERATORS_PARQUETSCANOPERATOR_H
