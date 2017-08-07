//
// Created by sabir on 04.07.17.
//

#ifndef CPP_MAPOPERATOR_H
#define CPP_MAPOPERATOR_H


#include <iostream>
#include "utils/tuple_to_string.h"
#include "Operator.h"

template<class Upstream, class Tuple, class MapFunction>
class MapOperator : public Operator {
public:
    Upstream *upstream;
    MapFunction mapFunction;

    MapOperator(Upstream *upstream1, MapFunction mapFunction) : upstream(upstream1), mapFunction(mapFunction) {}

    void printName() {
        upstream->printName();
        std::cout << "map op\n";
    }

    INLINE Optional<Tuple> next() {
        if (auto ret = upstream->next()) {
            auto meres = mapFunction(ret);
            DEBUG_PRINT(to_string(ret.value));
            DEBUG_PRINT(meres.v0);
            return meres;
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
