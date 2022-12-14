#ifndef CODE_GEN_OPERATORS_CONSTANTTUPLEOPERATOR_H
#define CODE_GEN_OPERATORS_CONSTANTTUPLEOPERATOR_H

#include "Utils.h"
#include "runtime/jit/operators/optional.hpp"

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

#endif  // CODE_GEN_OPERATORS_CONSTANTTUPLEOPERATOR_H
