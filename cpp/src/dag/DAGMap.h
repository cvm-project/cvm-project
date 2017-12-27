//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGMAP_H
#define DAG_DAGMAP_H

#include <iostream>

#include "DAGOperator.h"

class DAGVisitor;
class DAGMap : public DAGOperatorBase<DAGMap> {
public:
    void accept(DAGVisitor *v) override;

    constexpr static const char *kName = "map";
    std::string name() const override { return kName; }
};

#endif  // DAG_DAGMAP_H
