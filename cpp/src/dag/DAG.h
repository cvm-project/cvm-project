//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAG_H
#define DAG_DAG_H

#include <iostream>
#include <memory>
#include <vector>

#include "DAGOperator.h"

/**
 * destructor responsible for freeing the columns and the dag operators
 */
// TODO(sabir): refactor class in order to remove custom destructor
class DAG {  // NOLINT hicpp-special-member-functions
public:
    DAGOperator *sink{};
    std::vector<std::unique_ptr<DAGOperator>> operators;
    size_t last_operator_id{};

    ~DAG() { Column::delete_columns(); }
};

#endif  // DAG_DAG_H
