extern "C" {
#include "generate_dag_plan.h"
}

#include <cstddef>
#include <cstring>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "IR_analyzer/SchemaInference.h"
#include "code_gen/common/BackEnd.hpp"
#include "code_gen/cpp/BackEnd.hpp"
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

    // Select code gen back-end
    using CodeGenBackEnd = std::unique_ptr<code_gen::common::BackEnd>;
    std::unordered_map<std::string, CodeGenBackEnd> code_generators;
    code_generators.emplace("cpp",
                            CodeGenBackEnd(new code_gen::cpp::BackEnd()));

    const auto code_gen_backend = code_generators.at("cpp").get();

    // Generate code
    code_gen_backend->GenerateCode(dag.get());

    // Compile to native machine code
    TICK1
    // TODO(ingo): think again about this interface
    code_gen_backend->Compile(counter);
    TOCK1
    //    std::cout << "call make " << DIFF1 << std::endl;

    // TODO(ingo): better error handling
    return 0;
}
}  // extern "C"
