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
    const std::string genDir = get_lib_path() + "/backend/gen/";
    exec(("bash -c 'rm -r -f " + genDir + "/*'").c_str());

    // Create output directory
    const int dirErr = system(("mkdir -p " + genDir).c_str());
    if (0 != dirErr) {
        std::cerr << ("Error creating gen directory!") << std::endl;
        std::exit(1);
    }

    // Setup visitor and run it
    std::string llvmCodePath = genDir + "llvm_funcs.ll";
    std::ofstream llvmCode(llvmCodePath);

    std::stringstream declarations;
    std::stringstream definitions;

    std::unordered_map<std::string, size_t> unique_counters;
    std::set<std::string> includes;
    Context::TupleTypeRegistry tuple_type_descs;

    Context context(&declarations, &definitions, &llvmCode, &unique_counters,
                    &includes, &tuple_type_descs);

    auto execute_values = GenerateExecutePipelines(&context, dag);
    assert(execute_values == "execute_pipelines");

    // Main executable file: declarations
    std::ofstream mainSourceFile(genDir + "execute.cpp");

    mainSourceFile << "/**\n"
                      " * Auto-generated execution plan\n"
                      " */\n";

    includes.emplace("\"../../../runtime/free.hpp\"");

    for (const auto &incl : context.includes()) {
        mainSourceFile << "#include " << incl << std::endl;
    }

    mainSourceFile << "using namespace runtime::values;" << std::endl;

    mainSourceFile << declarations.str();
    mainSourceFile << definitions.str();

    // Main executable file: exported execute function
    mainSourceFile
            << "extern \"C\" { const char* execute("
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
    mainSourceFile <<  //
            "extern \"C\" {"
            "    void free_result(const char* const s) {"
            "        runtime::FreeValues(s);"
            "        free(const_cast<void *>("
            "                reinterpret_cast<const void*>(s)));"
            "    }"
            "}";

    // Header file
    std::ofstream headerFile(genDir + "execute.h");

    headerFile <<  //
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
