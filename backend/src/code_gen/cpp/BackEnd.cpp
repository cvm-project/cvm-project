#include "BackEnd.hpp"

#include <fstream>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

#include <sys/stat.h>

#include "CodeGenVisitor.h"
#include "dag/DAGOperators.h"
#include "dag/type/array.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/utils.h"

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

std::string ComputePlanDriver(const CodeGenVisitor::OperatorDesc &sink) {
    return (format("\n/** collecting the result **/"
                   "%1%.open();"
                   "auto result = %1%.next().value;"
                   "%1%.close();"
                   "return result;") %
            sink.var_name)
            .str();
}

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

    std::stringstream planBody;
    std::stringstream planTupleDeclarations;
    std::stringstream planLLVMDeclarations;

    CodeGenVisitor visitor(dag, planBody, planTupleDeclarations, llvmCode,
                           planLLVMDeclarations);
    dag::utils::ApplyInReverseTopologicalOrder(dag, visitor.functor());

    // Compute execute function parameters
    struct CollectInputsVisitor
        : public Visitor<CollectInputsVisitor, DAGOperator,
                         boost::mpl::list<DAGParameterLookup>> {
        void operator()(DAGParameterLookup *op) { inputs_.emplace_back(op); }
        std::vector<DAGParameterLookup *> inputs_;
    };
    CollectInputsVisitor collec_inputs_visitor;
    for (auto const op : dag->operators()) {
        collec_inputs_visitor.Visit(op);
    }

    std::vector<std::string> pack_input_args;
    std::vector<std::string> packed_input_args;
    for (auto const op : collec_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);

        // Packing of parameters as tules
        const auto tuple_type = op->tuple->type;
        const auto array_type = dynamic_cast<const dag::type::Array *>(
                tuple_type->field_types[0]);
        const auto inner_type = array_type->tuple_type;
        const auto tuple_typedef = visitor.tuple_type_descs_[tuple_type];
        const auto inner_typedef = visitor.tuple_type_descs_[inner_type];

        pack_input_args.emplace_back(
                (format("%1%{{reinterpret_cast<%2%*>("
                        "       inputs[%3%]->as<Array>()->data),"
                        "       inputs[%3%]->as<Array>()->shape[0]}}") %
                 tuple_typedef->name % inner_typedef->name % param_num)
                        .str());

        // Parameters for function signature of 'execute_plan' (as tuples)
        packed_input_args.emplace_back(
                (format("%1% input_%2%") % tuple_typedef->name % param_num)
                        .str());
    }

    // Generate typedef for result wrapper
    auto sink = visitor.operatorNameTupleTypeMap[dag->sink->id];

    // Main executable file: declarations
    std::ofstream mainSourceFile(genDir + "execute.cpp");

    mainSourceFile << "/**\n"
                      " * Auto-generated execution plan\n"
                      " */\n";

    for (const auto &incl : visitor.includes) {
        mainSourceFile << "#include " << incl << std::endl;
    }

    mainSourceFile                                                        //
            << "#include \"../../../runtime/values/array.hpp\"\n"         //
            << "#include \"../../../runtime/values/value.hpp\"\n"         //
            << "#include \"../../../runtime/values/json_parsing.hpp\"\n"  //
            << "using namespace runtime::values;" << std::endl;

    mainSourceFile << planTupleDeclarations.str();
    mainSourceFile << planLLVMDeclarations.str();

    // Main executable file: plan function on tuples
    mainSourceFile << format("%1% execute_tuples(%2%) { "
                             "    %3%\n%4% }") %
                              sink.return_type->name %
                              join(packed_input_args, ", ") % planBody.str() %
                              ComputePlanDriver(sink);

    // Main executable file: plan function on runtime values
    mainSourceFile <<  //
            format("VectorOfValues execute_values(const VectorOfValues &inputs)"
                   "{"
                   "    const auto ret = execute_tuples(%1%);"
                   "    std::unique_ptr<Array> val(new Array());"
                   "    val->data = ret.v0.data;"
                   "    val->shape = {ret.v0.shape[0]};"
                   "    VectorOfValues v;"
                   "    v.emplace_back(val.release());"
                   "    return v;"
                   " }") %
                    join(pack_input_args, ", ");

    // Main executable file: exported execute function
    mainSourceFile
            << "extern \"C\" { const char* execute("
               "        const char *const inputs_str) {"
               "    const auto inputs = ConvertFromJsonString(inputs_str);"
               "    const auto ret = execute_values(inputs);"
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
            "        const auto outputs = ConvertFromJsonString(s);"
            "        for (auto &output : outputs) {"
            "            free(output->as<Array>()->data);"
            "        }"
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
