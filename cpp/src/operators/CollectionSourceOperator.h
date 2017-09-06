//
// Created by sabir on 04.07.17.
//

#ifndef CPP_COLLECTIONSOURCEOPERATOR_H
#define CPP_COLLECTIONSOURCEOPERATOR_H

#include "Operator.h"
#include <cstring>

template<class Tuple, bool add_index = false>
class CollectionSourceOperator : public Operator {
public:

    CollectionSourceOperator(Tuple *vals, size_t size) : values(vals), size(size) {}

    INLINE void open() {
        index = 0;
    }

    INLINE Optional<Tuple> next() {
        if (index < size) {
            Tuple r = build_result();
            index++;
            return r;
        }
        return {};
    }

    INLINE void close() {
    }

private:
    const size_t size;
    size_t index;
    const void *values;

    INLINE Tuple build_result() {
        Tuple res;
        char *resp = (char *) &res;
        if (add_index) {
            size_t tuple_size = sizeof(Tuple) - sizeof(size_t);
            *((size_t *) resp) = index;
            memcpy(resp + sizeof(size_t), (char*)values + tuple_size * index, tuple_size);
        }
        else {
            *((Tuple *) resp) = ((Tuple *) values)[index];
        }
        return res;
    }
};

template<class Tuple>
CollectionSourceOperator<Tuple> makeCollectionSourceOperator(Tuple *data_ptr, size_t size) {
    return CollectionSourceOperator<Tuple>(data_ptr, size);
};

#endif //CPP_COLLECTIONSOURCEOPERATOR_H
