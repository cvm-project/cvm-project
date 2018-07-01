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
#include "dag/type/atomic.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "utils/utils.h"

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

std::string ComputeStructToValue(const std::string &input_var_name,
                                 const std::string &output_var_name) {
    return (format("std::unique_ptr<Array> %2%(new Array());"
                   "%2%->data = %1%.v0.data;"
                   "%2%->shape = {%1%.v0.shape[0]};") %
            input_var_name % output_var_name)
            .str();
}

std::string ComputeValueToStruct(
        const std::string &input_var_name, const dag::type::Tuple *tuple_type,
        const std::unordered_map<const dag::type::Type *,
                                 const CodeGenVisitor::StructDef *>
                &tuple_type_descs) {
    struct ValueToStructVisitor
        : public Visitor<ValueToStructVisitor, const dag::type::FieldType,
                         boost::mpl::list<                //
                                 const dag::type::Array,  //
                                 const dag::type::Atomic  //
                                 >::type,
                         std::string> {
        ValueToStructVisitor(
                // NOLINTNEXTLINE modernize-pass-by-value
                const std::string &input_var_name,
                const std::unordered_map<const dag::type::Type *,
                                         const CodeGenVisitor::StructDef *>
                        &tuple_type_descs)
            : input_var_name_(input_var_name),
              tuple_type_descs_(tuple_type_descs) {}

        std::string operator()(const dag::type::Array *const type) const {
            const auto inner_type = type->tuple_type;
            const auto inner_typedef = tuple_type_descs_.at(inner_type);

            return (format("{reinterpret_cast<%2%*>(%1%->as<Array>()->data),"
                           " %1%->as<Array>()->shape[0]}") %
                    input_var_name_ % inner_typedef->name)
                    .str();
        }

        std::string operator()(const dag::type::Atomic *const type) const {
            static const std::unordered_map<std::string, std::string> type_map =
                    {
                            {"float", "Float"},    //
                            {"double", "Double"},  //
                            {"int", "Int32"},      //
                            {"long", "Int64"},     //
                            {"bool", "Bool"}       //
                    };
            return (format("%1%->as<%2%>()->value") % input_var_name_ %
                    type_map.at(type->type))
                    .str();
        }

        std::string operator()(const dag::type::FieldType *const type) const {
            throw std::runtime_error("Unknown field type: " +
                                     type->to_string());
        }

        std::string input_var_name_;
        const std::unordered_map<const dag::type::Type *,
                                 const CodeGenVisitor::StructDef *>
                &tuple_type_descs_;
    };

    ValueToStructVisitor visitor("", tuple_type_descs);
    std::vector<std::string> field_values;
    for (size_t i = 0; i < tuple_type->field_types.size(); i++) {
        auto const &type = tuple_type->field_types[i];
        visitor.input_var_name_ = input_var_name + "->as<Tuple>()->fields[" +
                                  std::to_string(i) + "]";
        field_values.emplace_back(visitor.Visit(type));
    }

    const auto tuple_typedef = tuple_type_descs.at(tuple_type);
    return (format("%1%{%2%}") % tuple_typedef->name % join(field_values, ", "))
            .str();
}

std::string GenerateExecuteTuples(
        DAG *const dag, std::ostream &plan_tuple_declarations,
        std::ostream &plan_llvm_declarations, std::ostream &llvm_code,
        std::set<std::string> *const includes,
        std::unordered_map<const dag::type::Type *,
                           const CodeGenVisitor::StructDef *>
                *const tuple_type_descs) {
    std::stringstream plan_body;

    CodeGenVisitor visitor(dag, plan_body, plan_tuple_declarations, llvm_code,
                           plan_llvm_declarations);

    visitor.includes.insert(includes->begin(), includes->end());
    visitor.tuple_type_descs_.insert(tuple_type_descs->begin(),
                                     tuple_type_descs->end());

    dag::utils::ApplyInReverseTopologicalOrder(dag, visitor.functor());

    includes->insert(visitor.includes.begin(), visitor.includes.end());
    tuple_type_descs->insert(visitor.tuple_type_descs_.begin(),
                             visitor.tuple_type_descs_.end());

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

    std::vector<std::string> packed_input_args;
    for (auto const op : collec_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);
        const auto tuple_type = op->tuple->type;

        // Parameters for function signature of 'execute_tuples'
        const auto tuple_typedef = visitor.tuple_type_descs_.at(tuple_type);
        packed_input_args.emplace_back(
                (format("%1% input_%2%") % tuple_typedef->name % param_num)
                        .str());
    }

    auto sink = visitor.operatorNameTupleTypeMap[dag->sink->id];
    return (format("%1% execute_tuples(%2%) { "
                   "    %3%\n"
                   "    /** collecting the result **/"
                   "    %4%.open();"
                   "    auto result = %4%.next().value;"
                   "    %4%.close();"
                   "    return result;"
                   "}") %
            sink.return_type->name % join(packed_input_args, ", ") %
            plan_body.str() % sink.var_name)
            .str();
}

std::string GenerateExecuteValues(
        DAG *const dag, std::unordered_map<const dag::type::Type *,
                                           const CodeGenVisitor::StructDef *>
                                *const tuple_type_descs) {
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
    for (auto const op : collec_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);

        // Packing of parameters as tuples
        const auto tuple_type = op->tuple->type;
        pack_input_args.emplace_back(ComputeValueToStruct(
                "inputs[" + param_num + "]", tuple_type, *tuple_type_descs));
    }

    // Main executable file: plan function on runtime values
    return (format("VectorOfValues execute_values(const VectorOfValues &inputs)"
                   "{"
                   "    const auto ret = execute_tuples(%1%);"
                   "    %2%"
                   "    VectorOfValues v;"
                   "    v.emplace_back(val.release());"
                   "    return v;"
                   " }") %
            join(pack_input_args, ", ") % ComputeStructToValue("ret", "val"))
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

    std::stringstream planTupleDeclarations;
    std::stringstream planLLVMDeclarations;
    std::set<std::string> includes;
    std::unordered_map<const dag::type::Type *,
                       const CodeGenVisitor::StructDef *>
            tuple_type_descs;

    auto const execute_tuples_definition =  //
            GenerateExecuteTuples(dag, planTupleDeclarations,
                                  planLLVMDeclarations, llvmCode, &includes,
                                  &tuple_type_descs);
    auto const execute_values_definition =  //
            GenerateExecuteValues(dag, &tuple_type_descs);

    // Main executable file: declarations
    std::ofstream mainSourceFile(genDir + "execute.cpp");

    mainSourceFile << "/**\n"
                      " * Auto-generated execution plan\n"
                      " */\n";

    for (const auto &incl : includes) {
        mainSourceFile << "#include " << incl << std::endl;
    }

    mainSourceFile                                                        //
            << "#include \"../../../runtime/values/array.hpp\"\n"         //
            << "#include \"../../../runtime/values/atomics.hpp\"\n"       //
            << "#include \"../../../runtime/values/value.hpp\"\n"         //
            << "#include \"../../../runtime/values/json_parsing.hpp\"\n"  //
            << "using namespace runtime::values;" << std::endl;

    mainSourceFile << planTupleDeclarations.str();
    mainSourceFile << planLLVMDeclarations.str();
    mainSourceFile << execute_tuples_definition;
    mainSourceFile << execute_values_definition;

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
