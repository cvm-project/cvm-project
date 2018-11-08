#include "back_end.hpp"

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/process.hpp>

#include <sys/stat.h>

#include "code_gen.hpp"
#include "context.hpp"

namespace code_gen {
namespace cpp {

void BackEnd::Run(DAG* const dag) const {
    auto const config = nlohmann::json::parse(config_)["codegen"].flatten();

    const boost::filesystem::path temp_path_model =
            get_lib_path() / "backend/gen/build-%%%%-%%%%-%%%%-%%%%";

    // Create (empty) output directory
    auto const temp_dir = boost::filesystem::unique_path(temp_path_model);
    boost::filesystem::create_directories(temp_dir);

    auto const llvm_code_path = temp_dir / "llvm_funcs.ll";
    auto const source_file_path = temp_dir / "execute.cpp";
    auto const header_file_path = temp_dir / "execute.h";

    // Generate C++ code
    {
        // Setup visitor and run it
        boost::filesystem::ofstream llvm_code(llvm_code_path);

        std::stringstream declarations;
        std::stringstream definitions;

        std::unordered_map<std::string, size_t> unique_counters;
        std::set<std::string> includes;
        Context::TupleTypeRegistry tuple_type_descs;

        Context context(&declarations, &definitions, &llvm_code,
                        &unique_counters, &includes, &tuple_type_descs);

        auto execute_values = GenerateExecutePipelines(&context, dag);
        assert(execute_values == "execute_pipelines");

        // Main executable file: declarations
        boost::filesystem::ofstream source_file(source_file_path);

        source_file << "/**\n"
                       " * Auto-generated execution plan\n"
                       " */\n";

        includes.emplace("\"runtime/free.hpp\"");
        includes.emplace("<cstring>");

        for (const auto& incl : context.includes()) {
            source_file << "#include " << incl << std::endl;
        }

        source_file << "using namespace runtime::values;" << std::endl;

        source_file << declarations.str();
        source_file << definitions.str();

        // Main executable file: exported execute function
        source_file
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

    // Compute hash value of generated code
    boost::filesystem::current_path(temp_dir);

    boost::process::pipe pipe;
    boost::process::ipstream sha256sum_std_out;
    auto const sha256sum = boost::process::search_path("sha256sum");

    auto const ret1 = boost::process::system(
            sha256sum, source_file_path.filename(), header_file_path.filename(),
            llvm_code_path.filename(), boost::process::std_out > pipe);
    assert(ret1 == 0);

    auto std_in_pipe = boost::process::std_in < pipe;
    auto const ret2 =
            boost::process::system(sha256sum, "-", std_in_pipe,
                                   boost::process::std_out > sha256sum_std_out);
    assert(ret2 == 0);

    auto const hash =
            boost::lexical_cast<std::string>(sha256sum_std_out.rdbuf())
                    .substr(0, 32);

    // Rename temporary folder to name based on hash value
    boost::filesystem::path lib_dir =
            get_lib_path() / ("backend/gen/lib-" + hash);

    if (!boost::filesystem::exists(lib_dir)) {
        boost::filesystem::rename(temp_dir, lib_dir);
    } else {
        assert(boost::filesystem::is_directory(lib_dir));
        boost::filesystem::remove_all(temp_dir);
    }

    // Compile inside temporary directory
    boost::filesystem::current_path(lib_dir);

    auto const makefile_path =
            get_lib_path() / "backend/src/code_gen/cpp/Makefile";
    auto const make = boost::process::search_path("make");
    const bool do_debug_build = config.value("/debug", false);
    auto const flag = std::string("DEBUG=") + (do_debug_build ? "1" : "0");

    boost::process::ipstream make_std_out;
    boost::process::ipstream make_std_err;
    const int exit_code =
            boost::process::system(make, "-j", "-f", makefile_path, flag,
                                   boost::process::std_out > make_std_out,
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

    // Create symlinks for front-end
    auto const gen_dir = get_lib_path() / "backend/gen";
    const size_t counter = config.value("/counter", 0);

    auto const lib_path = lib_dir.filename() / "execute.so";
    auto const lib_symlink =
            gen_dir / ("execute" + std::to_string(counter) + ".so");
    boost::filesystem::remove(lib_symlink);
    boost::filesystem::create_symlink(lib_path, lib_symlink);

    auto const header_path = lib_dir.filename() / "execute.h";
    auto const header_symlink = gen_dir / "execute.h";
    boost::filesystem::remove(header_symlink);
    boost::filesystem::create_symlink(header_path, header_symlink);
}

}  // namespace cpp
}  // namespace code_gen
