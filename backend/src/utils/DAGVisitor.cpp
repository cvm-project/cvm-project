#include "DAGVisitor.h"

#include "dag/DAG.h"
#include "dag/DAGOperator.h"
#include "utils/debug_print.h"

#include <boost/function_output_iterator.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>

void DAGVisitor::StartVisit() {
    const auto reverse_graph = boost::make_reverse_graph(dag_->graph());
    boost::topological_sort(
            reverse_graph,
            boost::make_function_output_iterator([this](const auto v) {
                const auto op = this->dag_->to_operator(v);
                op->accept(this);
            }));
}
