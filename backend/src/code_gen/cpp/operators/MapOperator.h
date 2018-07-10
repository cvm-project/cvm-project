//
// Created by sabir on 04.07.17.
//

#ifndef CPP_MAPOPERATOR_H
#define CPP_MAPOPERATOR_H

#include "Operator.h"

template <class Upstream, class Tuple, class MapFunction>
class MapOperator : public Operator {
public:
    MapOperator(Upstream *const upstream, MapFunction map_function)
        : upstream_(upstream), map_function_(map_function) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<Tuple> next() {
        if (auto ret = upstream_->next()) {
            return map_function_(ret.value());
        }
        return {};
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    MapFunction map_function_;
};

template <class Tuple, class Upstream, class MapFunction>
MapOperator<Upstream, Tuple, MapFunction> makeMapOperator(
        Upstream *const upstream, MapFunction func) {
    return MapOperator<Upstream, Tuple, MapFunction>(upstream, func);
};

#endif  // CPP_MAPOPERATOR_H
