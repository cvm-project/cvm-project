#include "generate_executable.hpp"

#include <cstddef>
#include <cstring>
#include <memory>

#include <json.hpp>

#include "dag/DAG.h"
#include "optimize/optimizer.hpp"
#include "runtime/free.hpp"

int GenerateExecutable(const char *const conf, const char *const dagstr,
                       const unsigned long counter) {  // NOLINT
    auto conf_json = nlohmann::json::parse(conf).flatten();
    conf_json["/optimizer/optimizations/code_gen/counter"] = counter;
    conf_json.emplace("/optimizer/optimization-level", 2);

    std::unique_ptr<DAG> dag(ParseDag(dagstr));
    optimize::Optimizer opt(conf_json.unflatten().dump());
    opt.Run(dag.get());

    // TODO(ingo): better error handling
    return 0;
}

void FreeResult(const char *const s) {
    runtime::FreeValues(s);
    free(const_cast<void *>(reinterpret_cast<const void *>(s)));
}
