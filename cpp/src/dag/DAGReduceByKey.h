//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGREDUCEBYKEY_H
#define DAG_DAGREDUCEBYKEY_H

#include "DAGOperator.h"
class DAGVisitor;

class DAGReduceByKey : public DAGOperatorBase<DAGReduceByKey> {
public:
    void accept(DAGVisitor *v) override;

    constexpr static const char *kName = "reduce_by_key";
};

#endif  // DAG_DAGREDUCEBYKEY_H
