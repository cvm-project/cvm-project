//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGFILTER_H
#define DAG_DAGFILTER_H

#include "DAGOperator.h"

class DAGVisitor;

class DAGFilter : public DAGOperator {
public:
    static const std::string DAG_OP_NAME;

    static DAGOperator *make_dag_operator() { return new DAGFilter; };

    void accept(DAGVisitor *v);

    std::string get_name() { return "Filter_" + std::to_string(id); }

    DAGFilter *copy() {
        DAGFilter *filt = new DAGFilter;
        filt->fields = fields;
        filt->read_set = read_set;
        filt->llvm_ir = llvm_ir;
        filt->output_type = output_type;
        filt->id = ++DAGOperator::lastOperatorIndex;
        return filt;
    }
};

#endif  // DAG_DAGFILTER_H
