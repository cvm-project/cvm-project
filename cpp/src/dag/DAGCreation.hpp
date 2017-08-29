//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGCREATION_H
#define CPP_DAGCREATION_H


#include <iostream>
#include <unordered_map>
#include "DAGOperator.h"
#include "DAG.h"
#include "utils/constants.h"


typedef DAGOperator *(*make_dag_function)(void); // function pointer type
typedef std::unordered_map<std::string, make_dag_function> DAGOperatorsMap;

DAG *parse_dag(std::string dag);

DAG *parse(std::stringstream *);

DAGOperator *get_operator(std::string);

void load_operators();

vector<TupleField> parse_output_type(const string &);

#endif //CPP_DAGCREATION_H
