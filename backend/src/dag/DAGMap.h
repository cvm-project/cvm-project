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
    constexpr static const char *kName = "map";
};

#endif  // DAG_DAGMAP_H