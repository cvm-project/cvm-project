//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGCREATION_H
#define CPP_DAGCREATION_H

#include <iostream>
#include <unordered_map>

#include "DAG.h"
#include "DAGOperator.h"
#include "utils/constants.h"

typedef DAGOperator *(*make_dag_function)(void);  // function pointer type
typedef std::unordered_map<std::string, make_dag_function> DAGOperatorsMap;

DAG *parse_dag(const std::string &dagstr);

DAG *parse(std::stringstream *istream);

DAGOperator *get_operator(const std::string &opName);

void load_operators();

std::vector<TupleField> parse_output_type(const std::string &output);

#endif  // CPP_DAGCREATION_H
