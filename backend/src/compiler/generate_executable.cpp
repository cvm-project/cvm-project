#include "generate_executable.hpp"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <unordered_map>

#include <json.hpp>

#include "compiler/compiler.hpp"

extern "C" {
int GenerateExecutable(const char *const conf, const char *const dagstr,
                       const unsigned long counter) {  // NOLINT
    auto conf_json = nlohmann::json::parse(conf).flatten();
    conf_json["/backend/counter"] = counter;
    conf_json.emplace("/optimizer/optimization-level", 1);

    compiler::Compiler compiler(dagstr, conf_json.unflatten().dump());
    compiler.GenerateExecutable();

    // TODO(ingo): better error handling
    return 0;
}
}  // extern "C"
