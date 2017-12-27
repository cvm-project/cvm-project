//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCOLLECTION_H
#define DAG_DAGCOLLECTION_H

#include "DAGOperator.h"

class DAGCollection : public DAGOperatorBase<DAGCollection> {
public:
    bool add_index = false;

    constexpr static const char *kName = "collection_source";
    std::string name() const override { return kName; }

    void accept(DAGVisitor *v) override;
    void to_json(nlohmann::json *json) const override;
    void from_json(const nlohmann::json &json) override;
};

#endif  // DAG_DAGCOLLECTION_H
