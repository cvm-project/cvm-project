#ifndef CODE_GEN_OPERATORS_ZIPOPERATOR_H
#define CODE_GEN_OPERATORS_ZIPOPERATOR_H

#include <tuple>
#include <utility>

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

template <class Tuple, class... Upstreams>
class ZipOperator {
    using IndexSequence = std::make_index_sequence<sizeof...(Upstreams)>;

public:
    ZipOperator(Upstreams *...upstreams) : upstreams_(upstreams...){};

    template <std::size_t... I>
    void CloseUpstreams(std::index_sequence<I...>) {
        ((std::get<I>(upstreams_)->close()), ...);
    }

    template <std::size_t... I>
    void OpenUpstreams(std::index_sequence<I...>) {
        ((std::get<I>(upstreams_)->open()), ...);
    }

    template <std::size_t... I>
    Optional<Tuple> FetchNextFromUpstream(std::index_sequence<I...>) {
        const auto ret = std::make_tuple(std::get<I>(upstreams_)->next()...);
        if ((std::get<I>(ret) && ...)) {
            auto temp = std::tuple_cat(
                    TupleToStdTuple(std::get<I>(ret).value())...);
            return StdTupleToTuple(temp);
        }
        return {};
    }

    void INLINE open() { OpenUpstreams(IndexSequence()); }

    Optional<Tuple> INLINE next() {
        while (auto res = FetchNextFromUpstream(IndexSequence())) {
            return res;
        }
        return {};
    }

    void INLINE close() { CloseUpstreams(IndexSequence()); }

private:
    const std::tuple<Upstreams *...> upstreams_;
};

template <class Tuple, class... Upstreams>
ZipOperator<Tuple, Upstreams...> INLINE
makeZipOperator(Upstreams *...upstreams) {
    return ZipOperator<Tuple, Upstreams...>(upstreams...);
};

#endif  // CODE_GEN_OPERATORS_ZIPOPERATOR_H
