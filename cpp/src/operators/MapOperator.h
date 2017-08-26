//
// Created by sabir on 04.07.17.
//

#ifndef CPP_MAPOPERATOR_H
#define CPP_MAPOPERATOR_H

#include "Operator.h"

template<class Upstream, class Tuple, class MapFunction>
class MapOperator : public Operator {
public:
    Upstream *upstream;
    const MapFunction mapFunction;

    MapOperator(Upstream *upstream1, MapFunction mapFunction) : upstream(upstream1), mapFunction(mapFunction) {}

    INLINE Optional<Tuple> next() {
        if (auto ret = upstream->next()) {
//            auto meres = ;
//            return meres;
            return {{ret.value.v0, ret.value.v0*3+7}};
        }
        return {};
    }

    INLINE void open() { upstream->open(); }

    INLINE void close() {upstream->close();}
};

template<class Tuple, class Upstream, class MapFunction>
MapOperator<Upstream, Tuple, MapFunction> makeMapOperator(Upstream *upstream, MapFunction func) {
    return MapOperator<Upstream, Tuple, MapFunction>(upstream, func);
};

#endif //CPP_MAPOPERATOR_H
