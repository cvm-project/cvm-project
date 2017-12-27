//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGJOIN_H
#define DAG_DAGJOIN_H

#include <iostream>

#include "DAGOperator.h"

class DAGJoin : public DAGOperatorBase<DAGJoin> {
public:
    bool stream_right = true;

    void accept(DAGVisitor *v) override;

    constexpr static const char *kName = "join";
    std::string name() const override { return kName; }
};

#endif  // DAG_DAGJOIN_H
