#include "src/dag/DAGCreation.hpp"

int main(int argc, char **argv) {
    DAG *dag = parse_dag();
    Operator* op = init_dag(dag);
    op->printName();
    return 0;
}
