#ifndef CPP_CONSTANTTUPLEOPERATOR_H
#define CPP_CONSTANTTUPLEOPERATOR_H

#include "Optional.h"
#include "Utils.h"

template <class Tuple>
class ConstantTupleOperator {
public:
    ConstantTupleOperator(const Optional<Tuple> &tuple)
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
    const Optional<Tuple> tuple_;
    bool has_returned_;
};

template <class Tuple>
ConstantTupleOperator<Tuple> makeConstantTupleOperator(
        const Optional<Tuple> &tuple) {
    return ConstantTupleOperator<Tuple>(tuple);
};

#endif  // CPP_CONSTANTTUPLEOPERATOR_H
