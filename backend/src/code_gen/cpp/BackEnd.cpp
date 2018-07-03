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
#include "context.hpp"
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

std::string ComputeValueToStruct(const std::string &input_var_name,
                                 const dag::type::Tuple *tuple_type,
                                 Context *context) {
    struct ValueToStructVisitor
        : public Visitor<ValueToStructVisitor, const dag::type::FieldType,
                         boost::mpl::list<                //
                                 const dag::type::Array,  //
                                 const dag::type::Atomic  //
                                 >::type,
                         std::string> {
        ValueToStructVisitor(
                // NOLINTNEXTLINE modernize-pass-by-value
                const std::string &input_var_name, Context *const context)
            : input_var_name_(input_var_name), context_(context) {}

        std::string operator()(const dag::type::Array *const type) const {
            const auto inner_type = type->tuple_type;
            const auto inner_typedef =
                    context_->tuple_type_descs().at(inner_type);

            return (format("{reinterpret_cast<%2%*>(%1%->as<Array>()->data),"
                           " %1%->as<Array>()->shape[0]}") %
                    input_var_name_ % inner_typedef->name)
                    .str();
        }

        std::string operator()(const dag::type::Atomic *const type) const {
            static const std::unordered_map<std::string, std::string> type_map =
                    {
                            {"float", "Float"},        //
                            {"double", "Double"},      //
                            {"int", "Int32"},          //
                            {"long", "Int64"},         //
                            {"bool", "Bool"},          //
                            {"std::string", "String"}  //
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
        Context *const context_;
    };

    ValueToStructVisitor visitor("", context);
    std::vector<std::string> field_values;
    for (size_t i = 0; i < tuple_type->field_types.size(); i++) {
        auto const &type = tuple_type->field_types[i];
        visitor.input_var_name_ = input_var_name + "->as<Tuple>()->fields[" +
                                  std::to_string(i) + "]";
        field_values.emplace_back(visitor.Visit(type));
    }

    const auto tuple_typedef = context->tuple_type_descs().at(tuple_type);
    return (format("%1%{%2%}") % tuple_typedef->name % join(field_values, ", "))
            .str();
}

const StructDef *GenerateExecuteTuples(DAG *const dag, Context *const context) {
    std::stringstream plan_body;

    CodeGenVisitor visitor(dag, context, plan_body);
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

    std::vector<std::string> packed_input_args;
    for (auto const op : collec_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);
        const auto tuple_type = op->tuple->type;

        // Parameters for function signature of 'execute_tuples'
        const auto tuple_typedef = context->tuple_type_descs().at(tuple_type);
        packed_input_args.emplace_back(
                (format("%1% input_%2%") % tuple_typedef->name % param_num)
                        .str());
    }

    auto sink = visitor.operator_descs_[dag->sink->id];
    context->definitions() <<  //
            format("%1% execute_tuples(%2%) { "
                   "    %3%\n"
                   "    /** collecting the result **/"
                   "    %4%.open();"
                   "    auto result = %4%.next().value;"
                   "    %4%.close();"
                   "    return result;"
                   "}") %
                    sink.return_type->name % join(packed_input_args, ", ") %
                    plan_body.str() % sink.var_name;

    return sink.return_type;
}

void GenerateExecuteValues(DAG *const dag, Context *context) {
    context->includes().emplace("\"../../../runtime/values/array.hpp\"");
    context->includes().emplace("\"../../../runtime/values/atomics.hpp\"");
    context->includes().emplace("\"../../../runtime/values/value.hpp\"");
    context->includes().emplace("\"../../../runtime/values/json_parsing.hpp\"");

    context->declarations() << "using namespace runtime::values;" << std::endl;

    // Compute execute function parameters
    struct CollectInputsVisitor
        : public Visitor<CollectInputsVisitor, DAGOperator,
                         boost::mpl::list<DAGParameterLookup>> {
        void operator()(DAGParameterLookup *op) { inputs_.emplace_back(op); }
        std::vector<DAGParameterLookup *> inputs_;
    };
    CollectInputsVisitor collect_inputs_visitor;
    for (auto const op : dag->operators()) {
        collect_inputs_visitor.Visit(op);
    }

    std::vector<std::string> pack_input_args;
    for (auto const op : collect_inputs_visitor.inputs_) {
        const std::string param_num = std::to_string(op->parameter_num);

        // Packing of parameters as tuples
        const auto tuple_type = op->tuple->type;
        pack_input_args.emplace_back(ComputeValueToStruct(
                "inputs[" + param_num + "]", tuple_type, context));
    }

    // Main executable file: plan function on runtime values
    context->definitions() <<  //
            format("VectorOfValues execute_values(const VectorOfValues "
                   "&inputs)"
                   "{"
                   "    const auto ret = execute_tuples(%1%);"
                   "    %2%"
                   "    VectorOfValues v;"
                   "    v.emplace_back(val.release());"
                   "    return v;"
                   "}") %
                    join(pack_input_args, ", ") %
                    ComputeStructToValue("ret", "val");
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

    std::stringstream declarations;
    std::stringstream definitions;

    std::unordered_map<std::string, size_t> unique_counters;
    std::unordered_set<std::string> includes;
    Context::TupleTypeRegistry tuple_type_descs;

    Context context(&declarations, &definitions, &llvmCode, &unique_counters,
                    &includes, &tuple_type_descs);

    GenerateExecuteTuples(dag, &context);
    GenerateExecuteValues(dag, &context);

    // Main executable file: declarations
    std::ofstream mainSourceFile(genDir + "execute.cpp");

    mainSourceFile << "/**\n"
                      " * Auto-generated execution plan\n"
                      " */\n";

    for (const auto &incl : context.includes()) {
        mainSourceFile << "#include " << incl << std::endl;
    }

    mainSourceFile << declarations.str();
    mainSourceFile << definitions.str();

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
