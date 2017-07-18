//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGCREATION_H
#define CPP_DAGCREATION_H




#include <iostream>
#include <unordered_map>
#include "DAGOperator.h"
#include "DAG.h"

#define FILTER "filter"
#define MAP "map"
#define FLAT_MAP "flat_map"
#define JOIN "join"
#define RANGE "range_source"
#define COLLECTION "collection_source"
const std::string DAG_OP_RANGE = "range_source";
const std::string DAG_ACTION = "action";
const std::string DAG_DAG = "dag";
const std::string DAG_ID = "id";
const std::string DAG_OP = "op";
const std::string DAG_FUNC = "func";
const std::string DAG_OUTPUT_TYPE = "output_type";
const std::string DAG_PREDS = "predecessors";
const std::string DAG_VALUES = "values";
const std::string PLUGINS_DIR = "src/op_plugins";




typedef DAGOperator *(*make_dag_function)(void); // function pointer type
typedef std::unordered_map<std::string, make_dag_function> DAGOperatorsMap;

DAG *parse_dag(std::string filename = "./dag.json");
Operator *init_dag(DAG*);
DAG *parse(std::ifstream);
DAGOperator *get_operator(std::string);
void load_plugins();

#endif //CPP_DAGCREATION_H
