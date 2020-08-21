#ifndef CODE_GEN_OPERATORS_EXCHANGETCPOPERATOR_H
#define CODE_GEN_OPERATORS_EXCHANGETCPOPERATOR_H

#include "TupleToValueOperator.h"
#include "Utils.h"
#include "runtime/jit/operators/exchange_tcp.hpp"
#include "runtime/jit/operators/type_traits.hpp"

template <class Tuple>
class ExchangeTcpOperator {
public:
    ExchangeTcpOperator(
            std::unique_ptr<runtime::operators::ValueOperator> &&upstream)
        : upstream_(std::move(upstream)) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<Tuple> next() {
        const auto input = upstream_->next();
        return ValueToTuple<Tuple>(input);
    }

    INLINE void close() { upstream_->close(); }

private:
    std::unique_ptr<runtime::operators::ValueOperator> upstream_;
};

template <class OutputTuple, class Upstream, class... InputTypes>
auto makeExchangeTcpOperator(Upstream *const upstream, const size_t exchange_id,
                             const size_t num_levels, const size_t level_num,
                             std::tuple<long, InputTypes...> /*tag*/) {
    static constexpr const char *const kColumnTypes[sizeof...(InputTypes)] = {
            runtime::operators::TypeName<decltype(
                    std::declval<InputTypes>().data[0].v0)>::kValue...};

    std::vector<std::string> column_types(std::begin(kColumnTypes),
                                          std::end(kColumnTypes));

    return ExchangeTcpOperator<OutputTuple>(
            runtime::operators::MakeExchangeTcpOperator(
                    std::make_unique<TupleToValueOperator<Upstream>>(upstream),
                    std::move(column_types), exchange_id, num_levels,
                    level_num));
}

template <class OutputTuple, class Upstream>
auto makeExchangeTcpOperator(Upstream *const upstream, const size_t exchange_id,
                             const size_t num_levels, const size_t level_num) {
    using InputTuple = decltype(upstream->next().value());
    using StdInputTuple = decltype(TupleToStdTuple(std::declval<InputTuple>()));
    return makeExchangeTcpOperator<OutputTuple, Upstream>(
            upstream, exchange_id, num_levels, level_num, StdInputTuple());
};

#endif  // CODE_GEN_OPERATORS_EXCHANGETCPOPERATOR_H
