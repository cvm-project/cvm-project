//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCREATION_HPP
#define DAG_DAGCREATION_HPP

#include <iostream>
#include <string>

#include "DAG.h"
#include "DAGOperator.h"

DAGOperator *get_operator(const std::string &opName);

DAG *parse_dag(std::istream *istream);
DAG *parse_dag(const std::string &dagstr);

#endif  // DAG_DAGCREATION_HPP
