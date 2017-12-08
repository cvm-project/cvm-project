//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAG_H
#define CPP_DAG_H

#include <iostream>

#include "DAGOperator.h"

/**
 * destructor responsible for freeing the columns and the dag operators
 */
class DAG {
public:
    DAGOperator *sink;
    std::string action;

    ~DAG() {
        Column::delete_columns();
        delete (sink);
    }
};

#endif  // CPP_DAG_H
