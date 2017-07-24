#include <utility>
#include <cstddef>
#include <tuple>
#include <iostream>
#include "code_gen/generate_code.h"

#include "src/dag/DAGCreation.hpp"

int generate_execute(std::string dag) {
    //get the dag string as an argument
    DAG *dag = parse_dag(dag);
    generate_code(dag);
    //compile the code to some object file

    //return

    //in python call the execute() function
    
    //which should return a pointer to the result
    //which should be interpreted as specified by the user
    //e.g. list or an integer(if the action is count)
    return 0;
}
