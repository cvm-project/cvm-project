#include "BackEnd.hpp"

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

#include <sys/stat.h>

#include "code_gen.hpp"
#include "context.hpp"

namespace code_gen {
namespace cpp {

void BackEnd::GenerateCode(DAG *const dag) {
    const std::string gen_dir = get_lib_path() + "/backend/gen/";
    exec(("bash -c 'rm -r -f " + gen_dir + "/*'").c_str());

    // Create output directory
    const int dir_err = system(("mkdir -p " + gen_dir).c_str());
    if (0 != dir_err) {
        std::cerr << ("Error creating gen directory!") << std::endl;
        std::exit(1);
    }

    // Setup visitor and run it
    std::string llvm_code_path = gen_dir + "llvm_funcs.ll";
    std::ofstream llvm_code(llvm_code_path);

    std::stringstream declarations;
    std::stringstream definitions;

    std::unordered_map<std::string, size_t> unique_counters;
    std::set<std::string> includes;
    Context::TupleTypeRegistry tuple_type_descs;

    Context context(&declarations, &definitions, &llvm_code, &unique_counters,
                    &includes, &tuple_type_descs);

    auto execute_values = GenerateExecutePipelines(&context, dag);
    assert(execute_values == "execute_pipelines");

    // Main executable file: declarations
    std::ofstream source_file(gen_dir + "execute.cpp");

    source_file << "/**\n"
                   " * Auto-generated execution plan\n"
                   " */\n";

    includes.emplace("\"runtime/free.hpp\"");

    for (const auto &incl : context.includes()) {
        source_file << "#include " << incl << std::endl;
    }

    source_file << "using namespace runtime::values;" << std::endl;

    source_file << declarations.str();
    source_file << definitions.str();

    // Main executable file: exported execute function
    source_file << "extern \"C\" { const char* execute("
                   "        const char *const inputs_str) {"
                   "    const auto inputs = ConvertFromJsonString(inputs_str);"
                   "    const auto ret = execute_pipelines(inputs);"
                   "    const auto ret_str = ConvertToJsonString(ret);"
                   "    const auto ret_ptr = reinterpret_cast<char *>("
                   "            malloc(ret_str.size() + 1));"
                   "    strcpy(ret_ptr, ret_str.c_str());"
                   "    return ret_ptr;"
                   "} }";

    // Main executable file: free
    source_file <<  //
            "extern \"C\" {"
            "    void free_result(const char* const s) {"
            "        runtime::FreeValues(s);"
            "        free(const_cast<void *>("
            "                reinterpret_cast<const void*>(s)));"
            "    }"
            "}";

    // Header file
    std::ofstream header_file(gen_dir + "execute.h");

    header_file <<  //
            "const char* execute(const char* inputs_str);\n"
            "void free_result(const char*);";
}

void BackEnd::Compile(const uint64_t counter) {
    exec(("cd " + get_lib_path() +
          "/backend/gen && make LIB_ID=" + std::to_string(counter) + " -f " +
          get_lib_path() + "/backend/src/code_gen/cpp/Makefile -j")
                 .c_str());
}

}  // namespace cpp
}  // namespace code_gen
