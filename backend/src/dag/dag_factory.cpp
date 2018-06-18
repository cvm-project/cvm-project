
#include "dag_factory.hpp"

#include "DAGOperators.h"

DAGOperator *DagFactory::MakeOperator(const std::string &opName) {
    if (instance().op_map_.count(opName) == 0) {
        throw std::invalid_argument("operator " + opName +
                                    " could not be found");
    }
    return instance().op_map_[opName]();
}
DAG *DagFactory::ParseDag(std::istream *istream) {
    nlohmann::json json;
    (*istream) >> json;
    auto dag = json.get<std::unique_ptr<DAG>>();

    return dag.release();
}
DAG *DagFactory::ParseDag(const std::string &dagstr) {
    std::stringstream stream(dagstr);
    return ParseDag(&stream);
}

void DagFactory::load_operators() {
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

template <class OperatorType>
void DagFactory::load_operator() {
    std::string name(OperatorType::kName);
    op_map_.emplace(name.c_str(), &OperatorType::make_dag_operator);
}
