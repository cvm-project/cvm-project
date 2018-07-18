//
// Created by sabir on 04.07.17.
//

#ifndef CPP_COLLECTIONSOURCEOPERATOR_H
#define CPP_COLLECTIONSOURCEOPERATOR_H

#include <cstring>

#include "Operator.h"

template <class Tuple, bool kAddIndex, class Upstream>
class CollectionSourceOperator : public Operator {
public:
    CollectionSourceOperator(Upstream *const upstream) : upstream_(upstream) {}

    INLINE void open() {
        upstream_->open();
        current_index_ = 0;
        last_index_ = 0;
        values_ = nullptr;
    }

    INLINE Optional<Tuple> next() {
        while (current_index_ >= last_index_) {
            const auto ret = upstream_->next();
            if (!ret) return {};

            // XXX: This is currently a memory leak! We need a clear way to
            //      manage memory that releases allocated memory as soon as it
            //      is not used
            const auto input = ret.value().v0;
            current_index_ = input.offsets[0];
            last_index_ = current_index_ + input.shape[0];
            values_ = input.data;
        }

        Tuple r = BuildResult();
        current_index_++;
        return r;
    }

    INLINE void close() { upstream_->close(); }

private:
    Upstream *const upstream_;
    size_t current_index_;
    size_t last_index_;
    void *values_;

    INLINE Tuple BuildResult() {
        Tuple res;
        char *resp = (char *)&res;
        if (kAddIndex) {
            size_t tuple_size = sizeof(Tuple) - sizeof(size_t);
            *((size_t *)resp) = current_index_;
            memcpy(resp + sizeof(size_t),
                   (char *)values_ + tuple_size * current_index_, tuple_size);
        } else {
            *((Tuple *)resp) = ((Tuple *)values_)[current_index_];
        }
        return res;
    }
};

template <class Tuple, bool kAddIndex, class Upstream>
CollectionSourceOperator<Tuple, kAddIndex, Upstream>
makeCollectionSourceOperator(Upstream *const upstream) {
    return CollectionSourceOperator<Tuple, kAddIndex, Upstream>(upstream);
}

#endif  // CPP_COLLECTIONSOURCEOPERATOR_H
