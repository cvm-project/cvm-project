//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAG_H
#define DAG_DAG_H

#include <iostream>

#include "DAGOperator.h"

/**
 * destructor responsible for freeing the columns and the dag operators
 */
class DAG {
public:
    DAGOperator *sink{};
    std::string action;

    ~DAG() {
        Column::delete_columns();
        delete (sink);
    }
};

#endif  // DAG_DAG_H
