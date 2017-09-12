#include <utility>
#include <cstddef>
#include <tuple>
#include <iostream>
#include <cstring>
#include "code_gen/generate_code.h"
#include "utils/timing.h"
#include "IR_analyzer/SchemaInference.h"

#include "dag/DAGCreation.hpp"
#include "utils/printDAG.h"
#include "optimize/Optimizer.h"
#include "utils/utils.h"


extern "C" {
int generate_dag_plan(char *dagstr,  unsigned long counter) {

    exec(("rm -r -f " + get_lib_path() + "cpp/gen").c_str());

    DAG *dag = parse_dag(std::string(dagstr));
    //scheme inference
//    printDAG(dag);
    SchemaInference si;
    si.start_visit(dag);
//    printDAG(dag);
    //optimize
    Optimizer opt;
    opt.run(dag);
//    printDAG(dag);
    //generate code
    generate_code(dag);
    //call make in the subdir

    TICK1
    exec(("cd " + get_lib_path() + "cpp/gen && make LIB_ID=" + to_string(counter) + " -f ../src/utils/Makefile -j").c_str());
    TOCK1
//    std::cout << "call make " << DIFF1 << std::endl;
    delete (dag);
    return 0;
}

}