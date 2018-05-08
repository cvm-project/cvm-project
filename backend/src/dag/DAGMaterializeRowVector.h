#ifndef DAG_DAGMATERIALIZEROWVECTOR_H
#define DAG_DAGMATERIALIZEROWVECTOR_H

#include "DAGOperator.h"

class DAGMaterializeRowVector
    : public DAGOperatorBase<DAGMaterializeRowVector> {
public:
    constexpr static const char *kName = "materialize_row_vector";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGMATERIALIZEROWVECTOR_H
