
#include "DAGCreation.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <dirent.h>

#include "DAGOperators.h"
#include "utils/utils.h"

// TODO(sabir): Implement proper singleton pattern
using make_dag_function = DAGOperator *(*)();  // function pointer type
using DAGOperatorsMap = std::unordered_map<std::string, make_dag_function>;
static DAGOperatorsMap opMap;  // NOLINT

template <class OperatorType>
void load_operator() {
    std::string name(OperatorType::kName);
    opMap.emplace(name.c_str(), &OperatorType::make_dag_operator);
}

void load_operators() {
    load_operator<DAGCartesian>();
    load_operator<DAGCollection>();
    load_operator<DAGFilter>();
    load_operator<DAGJoin>();
    load_operator<DAGMap>();
    load_operator<DAGRange>();
    load_operator<DAGReduce>();
    load_operator<DAGReduceByKey>();
    load_operator<DAGReduceByKeyGrouped>();
}

DAGOperator *get_operator(const std::string &opName) {
    if (opMap.count(opName) == 0) {
        throw std::invalid_argument("operator " + opName +
                                    " could not be found");
    }
    return opMap[opName]();
}

DAG *parse_dag(std::istream *istream) {
    load_operators();

    nlohmann::json json;
    (*istream) >> json;
    auto dag = json.get<std::unique_ptr<DAG>>();

    return dag.release();
}

DAG *parse_dag(const std::string &dagstr) {
    std::stringstream stream(dagstr);
    return parse_dag(&stream);
};
