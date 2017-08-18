//
// Created by sabir on 04.07.17.
//

#ifndef CPP_COLLECTIONSOURCEOPERATOR_H
#define CPP_COLLECTIONSOURCEOPERATOR_H

#include "Operator.h"

template<class Tuple>
class CollectionSourceOperator : public Operator {
public:

    CollectionSourceOperator(Tuple *vals, size_t size) : values(vals), size(size) {}

    void open() {
        index = 0;
    }

    Optional<Tuple> next() {
        if (index < size) {
            auto r = values[index];
            index++;
            return r;
        }
        return {};
    }

    void close() {
    }

private:
    size_t size;
    size_t index;
    Tuple *values;
};

template<class Tuple>
CollectionSourceOperator<Tuple> makeCollectionSourceOperator(Tuple *data_ptr, size_t size) {
    return CollectionSourceOperator<Tuple>(data_ptr, size);
};

#endif //CPP_COLLECTIONSOURCEOPERATOR_H
