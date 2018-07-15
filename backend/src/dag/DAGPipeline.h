#ifndef DAG_DAGPIPELINE_H
#define DAG_DAGPIPELINE_H

#include "DAG.h"
#include "DAGOperator.h"

class DAGPipeline : public DAGOperatorBase<DAGPipeline> {
public:
    constexpr static const char *kName = "pipeline";
    constexpr static size_t kNumInPorts = 0;
    constexpr static size_t kNumOutPorts = 1;

    size_t num_in_ports() const override { return num_inputs; }

    size_t num_inputs{};
};

#endif  // DAG_DAGPIPELINE_H
