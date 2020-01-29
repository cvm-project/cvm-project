#ifndef CODE_GEN_OPERATORS_EXCHANGES3OPERATOR_H
#define CODE_GEN_OPERATORS_EXCHANGES3OPERATOR_H

#include "TupleToValueOperator.h"
#include "Utils.h"
#include "runtime/jit/operators/exchange_s3.hpp"
#include "runtime/jit/operators/type_traits.hpp"

template <class Tuple>
class ExchangeS3Operator {
public:
    ExchangeS3Operator(
            std::unique_ptr<runtime::operators::ValueOperator> &&main_upstream)
        : upstream_(std::move(main_upstream)) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<Tuple> next() {
        const auto input = upstream_->next();
        return ValueToTuple<Tuple>(input);
    }

    INLINE void close() { upstream_->close(); }

private:
    std::unique_ptr<runtime::operators::ValueOperator> upstream_;
};

template <class OutputTuple, class MainUpstream, class DopUpstream,
          class WidUpstream, class... InputTypes>
auto makeExchangeS3Operator(MainUpstream *const main_upstream,
                            DopUpstream *const dop_upstream,
                            WidUpstream *const wid_upstream,
                            const size_t exchange_id, const size_t num_levels,
                            const size_t level_num,
                            std::tuple<long, InputTypes...> /*tag*/) {
    static constexpr const char *const kColumnTypes[sizeof...(InputTypes)] = {
            runtime::operators::TypeName<decltype(
                    std::declval<InputTypes>().data[0].v0)>::kValue...};

    std::vector<std::string> column_types(std::begin(kColumnTypes),
                                          std::end(kColumnTypes));

    return ExchangeS3Operator<
            OutputTuple>(runtime::operators::MakeExchangeS3Operator(
            std::make_unique<TupleToValueOperator<MainUpstream>>(main_upstream),
            std::make_unique<TupleToValueOperator<DopUpstream>>(dop_upstream),
            std::make_unique<TupleToValueOperator<WidUpstream>>(wid_upstream),
            std::move(column_types), exchange_id, num_levels, level_num));
}

template <class OutputTuple, class MainUpstream, class DopUpstream,
          class WidUpstream>
auto makeExchangeS3Operator(MainUpstream *const main_upstream,
                            DopUpstream *const dop_upstream,
                            WidUpstream *const wid_upstream,
                            const size_t exchange_id, const size_t num_levels,
                            const size_t level_num) {
    using InputTuple = decltype(main_upstream->next().value());
    using StdInputTuple = decltype(TupleToStdTuple(std::declval<InputTuple>()));
    return makeExchangeS3Operator<OutputTuple, MainUpstream, DopUpstream,
                                  WidUpstream>(
            main_upstream, dop_upstream, wid_upstream, exchange_id, num_levels,
            level_num, StdInputTuple());
};

#endif  // CODE_GEN_OPERATORS_EXCHANGES3OPERATOR_H
