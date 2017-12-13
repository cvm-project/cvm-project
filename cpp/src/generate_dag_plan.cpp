extern "C" {
#include "generate_dag_plan.h"
}

#include <cstddef>
#include <cstring>
#include <iostream>
#include <tuple>
#include <utility>

#include "IR_analyzer/SchemaInference.h"
#include "code_gen/generate_code.h"
#include "dag/DAGCreation.hpp"
#include "optimize/Optimizer.h"
#include "utils/printDAG.h"
#include "utils/timing.h"
#include "utils/utils.h"

extern "C" {
int generate_dag_plan(char *dagstr, unsigned long counter) {  // NOLINT
    exec(("bash -c 'rm -r -f " + get_lib_path() + "/cpp/gen/*'").c_str());

    DAG *dag = parse_dag(std::string(dagstr));
    // scheme inference
    //    printDAG(dag);
    SchemaInference si;
    si.start_visit(dag);
    //    printDAG(dag);
    // optimize
    Optimizer opt;
    opt.run(dag);
    //    printDAG(dag);
    // generate code
    generate_code(dag);
    // call make in the subdir

    TICK1
    exec(("cd " + get_lib_path() +
          "/cpp/gen && make LIB_ID=" + std::to_string(counter) + " -f " +
          get_lib_path() + "/cpp/src/operators/Makefile -j")
                 .c_str());
    TOCK1
    //    std::cout << "call make " << DIFF1 << std::endl;
    delete (dag);
    return 0;
}
}