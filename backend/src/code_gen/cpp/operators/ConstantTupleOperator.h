#ifndef CPP_CONSTANTTUPLEOPERATOR_H
#define CPP_CONSTANTTUPLEOPERATOR_H

#include <cstring>

#include "Operator.h"

template <class Tuple>
class ConstantTupleOperator : public Operator {
public:
    ConstantTupleOperator(const Tuple &tuple)
        : tuple_(tuple), has_returned_(false) {}

    INLINE void open() { has_returned_ = false; }
    INLINE Optional<Tuple> next() {
        if (!has_returned_) {
            has_returned_ = true;
            return tuple_;
        }
        return {};
    }
    INLINE void close() {}

private:
    const Tuple tuple_;
    bool has_returned_;
};

template <class Tuple>
ConstantTupleOperator<Tuple> makeConstantTupleOperator(const Tuple &tuple) {
    return ConstantTupleOperator<Tuple>(tuple);
};

#endif  // CPP_CONSTANTTUPLEOPERATOR_H
