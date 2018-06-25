//
// Created by sabir on 04.07.17.
//

#ifndef CPP_COLLECTIONSOURCEOPERATOR_H
#define CPP_COLLECTIONSOURCEOPERATOR_H

#include <cstring>

#include "Operator.h"

template <class Tuple, bool add_index, class Upstream>
class CollectionSourceOperator : public Operator {
public:
    CollectionSourceOperator(Upstream *const upstream) : upstream(upstream) {}

    INLINE void open() {
        upstream->open();
        auto input_tuple = upstream->next().value;
        upstream->close();
        // XXX: This is currently a memory leak! We need a clear way to manage
        //      memory that releases allocated memory as soon as it is not used
        values = input_tuple.v0.data;
        size = input_tuple.v0.shape[0];
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

    INLINE void close() {}

private:
    Upstream *const upstream;
    size_t size;
    size_t index;
    void *values;

    INLINE Tuple build_result() {
        Tuple res;
        char *resp = (char *)&res;
        if (add_index) {
            size_t tuple_size = sizeof(Tuple) - sizeof(size_t);
            *((size_t *)resp) = index;
            memcpy(resp + sizeof(size_t), (char *)values + tuple_size * index,
                   tuple_size);
        } else {
            *((Tuple *)resp) = ((Tuple *)values)[index];
        }
        return res;
    }
};

template <class Tuple, bool add_index, class Upstream>
CollectionSourceOperator<Tuple, add_index, Upstream>
makeCollectionSourceOperator(Upstream *const upstream) {
    return CollectionSourceOperator<Tuple, add_index, Upstream>(upstream);
}

#endif  // CPP_COLLECTIONSOURCEOPERATOR_H
