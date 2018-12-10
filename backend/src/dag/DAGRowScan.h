//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGROWSCAN_H
#define DAG_DAGROWSCAN_H

#include "DAGOperator.h"

class DAGRowScan : public DAGOperatorBase<DAGRowScan> {
public:
    bool add_index = false;

    constexpr static const char *kName = "collection_source";
    constexpr static size_t kNumInPorts = 1;
    constexpr static size_t kNumOutPorts = 1;

    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;
};

#endif  // DAG_DAGROWSCAN_H
