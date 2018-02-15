extern "C" {
#include "generate_dag_plan.h"
}

#include <cstddef>
#include <cstring>
#include <iostream>
#include <tuple>
#include <utility>

#include "IR_analyzer/SchemaInference.h"
#include "code_gen/cpp/generate_code.h"
#include "dag/DAGCreation.hpp"
#include "optimize/Optimizer.h"
#include "utils/printDAG.h"
#include "utils/timing.h"
#include "utils/utils.h"

extern "C" {
int generate_dag_plan(const char *const dagstr,
                      const unsigned long counter) {  // NOLINT

    // Parse DAG
    std::unique_ptr<DAG> dag(parse_dag(std::string(dagstr)));

    // Schema inference
    SchemaInference si;
    si.visitDag(dag.get());

    // Optimize
    Optimizer opt;
    opt.run(dag.get());

    // Generate code
    exec(("bash -c 'rm -r -f " + get_lib_path() + "/cpp/gen/*'").c_str());
    generate_code(dag.get());

    // Call make in the subdir
    TICK1
    exec(("cd " + get_lib_path() +
          "/cpp/gen && make LIB_ID=" + std::to_string(counter) + " -f " +
          get_lib_path() + "/cpp/src/operators/Makefile -j")
                 .c_str());
    TOCK1
    //    std::cout << "call make " << DIFF1 << std::endl;

    return 0;
}
}  // extern "C"
