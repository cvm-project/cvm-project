#ifndef DAG_DAGASSERTCORRECTOPENNEXTCLOSE_H
#define DAG_DAGASSERTCORRECTOPENNEXTCLOSE_H

#include "DAGOperator.h"

class DAGAssertCorrectOpenNextClose : public DAGOperator {
    JITQ_DAGOPERATOR(DAGAssertCorrectOpenNextClose,
                     "assert_correct_open_next_close");

public:
    size_t num_in_ports() const override {
        return 1;
    }  // XXX: make this dynamic
    size_t num_out_ports() const override { return 1; }
};

#endif  // DAG_DAGASSERTCORRECTOPENNEXTCLOSE_H
