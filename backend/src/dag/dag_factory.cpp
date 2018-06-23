
#include "dag_factory.hpp"

#include <boost/mpl/for_each.hpp>

#include "DAGOperators.h"
#include "dag/all_operator_declarations.hpp"

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
    boost::mpl::for_each<dag::AllOperatorPointerTypes>(
            LoadOperatorFunctor{this});
}

template <class OperatorType>
void DagFactory::LoadOperatorFunctor::operator()(OperatorType * /*unused*/) {
    std::string name(OperatorType::kName);
    factory->op_map_.emplace(name.c_str(), &OperatorType::make_dag_operator);
}
