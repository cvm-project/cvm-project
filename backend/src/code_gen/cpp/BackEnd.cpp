#include "BackEnd.hpp"

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/process.hpp>

#include <sys/stat.h>

#include "code_gen.hpp"
#include "context.hpp"

namespace code_gen {
namespace cpp {

void BackEnd::GenerateCode(DAG *const dag) {
    // Create (empty) output directory
    auto const gen_dir = get_lib_path() / "backend/gen";
    boost::filesystem::remove_all(gen_dir);
    boost::filesystem::create_directory(gen_dir);

    auto const llvm_code_path = gen_dir / "llvm_funcs.ll";
    auto const source_file_path = gen_dir / "execute.cpp";
    auto const header_file_path = gen_dir / "execute.h";

    // Setup visitor and run it
    boost::filesystem::ofstream llvm_code(llvm_code_path);

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
    boost::filesystem::ofstream source_file(source_file_path);

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
    boost::filesystem::ofstream header_file(header_file_path);

    header_file <<  //
            "const char* execute(const char* inputs_str);\n"
            "void free_result(const char*);";
}

void BackEnd::Compile(const uint64_t counter) {
    auto const gen_dir = get_lib_path() / "backend/gen";
    boost::filesystem::current_path(gen_dir);

    auto const makefile_path =
            get_lib_path() / "backend/src/code_gen/cpp/Makefile";
    auto const make = boost::process::search_path("make");

    boost::process::ipstream make_std_out;
    boost::process::ipstream make_std_err;
    const int exit_code = boost::process::system(
            make, "LIB_ID=" + std::to_string(counter), "-j", "-f",
            makefile_path, boost::process::std_out > make_std_out,
            boost::process::std_err > make_std_err);

    if (exit_code != 0) {
        throw std::runtime_error(
                (boost::format(
                         "Error while generating library (exit code %1%).\n\n"
                         "--- stdout: ---------------------------\n%2%"
                         "--- stderr: ---------------------------\n%3%") %
                 exit_code % make_std_out.rdbuf() % make_std_err.rdbuf())
                        .str());
    }
}

}  // namespace cpp
}  // namespace code_gen
