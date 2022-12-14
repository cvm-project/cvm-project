#include "code_gen/code_gen.hpp"

#include <algorithm>
#include <map>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/process.hpp>

#include "code_gen_visitor.hpp"
#include "dag/operators/all_operator_definitions.hpp"
#include "dag/type/array.hpp"
#include "dag/type/atomic.hpp"
#include "dag/utils/apply_visitor.hpp"
#include "llvm_helpers/function.hpp"
#include "utils/lib_path.hpp"

using boost::format;
using boost::algorithm::join;

namespace code_gen::cpp {

auto GenerateCode(DAG *const dag, const std::string &config)
        -> std::pair<std::string, std::string> {
    auto const jconfig = nlohmann::json::parse(config).flatten();

    const boost::filesystem::path temp_path_model =
            get_lib_path() / "backend/gen/build-%%%%-%%%%-%%%%-%%%%";

    // Create (empty) output directory
    auto const temp_dir = boost::filesystem::unique_path(temp_path_model);
    boost::filesystem::create_directories(temp_dir);
    boost::filesystem::current_path(temp_dir);

    auto const llvm_code_dir = temp_dir / "llvm_funcs";
    auto const source_file_path = temp_dir / "execute.cpp";

    std::string function_name;
    std::vector<std::string> llvm_code_files;

    // Generate C++ code
    {
        // Setup visitor and run it
        boost::filesystem::create_directories(llvm_code_dir);

        std::stringstream declarations;
        std::stringstream definitions;

        std::unordered_map<std::string, size_t> unique_counters;
        std::set<std::string> includes;
        Context::TupleTypeRegistry tuple_type_descs;

        Context context(&declarations, &definitions,
                        llvm_code_dir.filename().string(), &llvm_code_files,
                        &unique_counters, &includes, &tuple_type_descs);

        function_name = GenerateExecutePipelines(&context, dag);

        std::sort(llvm_code_files.begin(), llvm_code_files.end());

        // Main executable file: declarations
        boost::filesystem::ofstream source_file(source_file_path);

        source_file << "/**\n"
                       " * Auto-generated execution plan\n"
                       " */\n";
        // Include AllHeaders first such that header precompilation works
        source_file << "#include \"AllHeaders.h\"" << std::endl;

        includes.emplace("<cstring>");

        for (const auto &incl : context.includes()) {
            source_file << "#include " << incl << std::endl;
        }

        source_file <<  //
                "using namespace runtime::values;"
                "using runtime::memory::RefCounter;"
                "using runtime::memory::SharedPointer;"
                    << std::endl;

        source_file << declarations.str();
        source_file << definitions.str();
    }

    // Compute hash value of generated code
    boost::process::pipe pipe;
    boost::process::ipstream sha256sum_std_out;
    auto const sha256sum = boost::process::search_path("sha256sum");

    // NOLINTNEXTLINE(clang-analyzer-core.NonNullParamChecker)  Boost problem
    auto const ret1 = boost::process::system(
            sha256sum, source_file_path.filename(), llvm_code_files,
            boost::process::std_out > pipe);
    assert(ret1 == 0);

    auto std_in_pipe = boost::process::std_in < pipe;
    auto const ret2 =
            boost::process::system(sha256sum, "-", std_in_pipe,
                                   boost::process::std_out > sha256sum_std_out);
    assert(ret2 == 0);

    // cppcheck-suppress unreadVariable
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
            get_lib_path() / "backend/src/generate/src/code_gen/Makefile";
    auto const make = boost::process::search_path("make");
    const bool do_debug_build = jconfig.value("/debug", false);
    auto const debug_flag =
            std::string("DEBUG=") + (do_debug_build ? "1" : "0");

#if defined(WITH_LLVM_ASAN)
    const auto *const asan_flag = "WITH_LLVM_ASAN=1";
#else
    const auto *const asan_flag = "WITH_LLVM_ASAN=0";
#endif

    boost::process::ipstream make_std_out;
    boost::process::ipstream make_std_err;
    const int exit_code = boost::process::system(
            make, "-j", "-f", makefile_path, debug_flag, asan_flag,
            boost::process::std_out > make_std_out,
            boost::process::std_err > make_std_err);

    if (exit_code != 0) {
        throw std::runtime_error(
                (boost::format("Error while generating library lib-%4% (exit "
                               "code %1%).\n\n"
                               "--- stdout: ---------------------------\n%2%"
                               "--- stderr: ---------------------------\n%3%") %
                 exit_code % make_std_out.rdbuf() % make_std_err.rdbuf() % hash)
                        .str());
    }

    // Return path of produced library
    auto const lib_name = std::string("libexecute.") +
                          (do_debug_build ? "dbg" : "opt") + ".so";
    return {(lib_dir.filename() / lib_name).string(), function_name};
}

auto AtomicTypeNameToRuntimeTypename(const std::string &type_name)
        -> std::string {
    static const std::unordered_map<std::string, std::string> type_map = {
            {"float", "Float"},        //
            {"double", "Double"},      //
            {"int", "Int32"},          //
            {"long", "Int64"},         //
            {"bool", "Bool"},          //
            {"std::string", "String"}  //
    };
    return type_map.at(type_name);
}

void GenerateTupleToValue(Context *const context,
                          const dag::type::Tuple *const tuple_type) {
    struct TupleToValueVisitor
        : public Visitor<TupleToValueVisitor, const dag::type::FieldType,
                         boost::mpl::list<                //
                                 const dag::type::Array,  //
                                 const dag::type::Atomic  //
                                 >::type,
                         std::string> {
        TupleToValueVisitor(std::ostream &output, Context *const context,
                            // NOLINTNEXTLINE(modernize-pass-by-value)
                            const std::string &input_var_name)
            : input_var_name_(input_var_name),
              output_(output),
              context_(context) {}

        auto operator()(const dag::type::Atomic *const type) const
                -> std::string {
            const auto temp_var_name = context_->GenerateSymbolName("val");
            output_ << format("std::unique_ptr<%2%> %1%(new %2%());"
                              "%1%->value = %3%;") %
                               temp_var_name %
                               AtomicTypeNameToRuntimeTypename(type->type) %
                               input_var_name_;
            return temp_var_name + ".release()";
        }

        auto operator()(const dag::type::Array * /*type*/) const
                -> std::string {
            const auto temp_var_name = context_->GenerateSymbolName("val");
            output_ << format("std::unique_ptr<Array> %2%(new Array());"
                              "%2%->data = %1%.data.as<char>();"
                              "%2%->outer_shape = {%1%.outer_shape[0]};"
                              "%2%->offsets = {%1%.offsets[0]};"
                              "%2%->shape = {%1%.shape[0]};") %
                               input_var_name_ % temp_var_name;
            return temp_var_name + ".release()";
        }

    private:
        std::ostream &output_;
        Context *const context_;
        const std::string input_var_name_;
    };

    auto *const tuple_type_desc =
            context->tuple_type_descs().at(tuple_type).get();

    if (tuple_type_desc->has_struct_to_value) return;
    tuple_type_desc->has_struct_to_value = true;

    std::stringstream function_body;
    for (size_t i = 0; i < tuple_type->field_types.size(); i++) {
        const auto &type = tuple_type->field_types[i];
        const auto field_var_name = "t.value().v" + std::to_string(i);
        TupleToValueVisitor visitor(function_body, context, field_var_name);
        function_body << format("ret->fields.emplace_back(%1%);") %
                                 visitor.Visit(type);
    }

    context->definitions() <<  //
            format("template <>"
                   "std::shared_ptr<Value> TupleToValue("
                   "        const Optional<%1%>& t) {"
                   "    if (!t) {"
                   "        return std::make_shared<None>();"
                   "    }"
                   "    auto ret = std::make_shared<Tuple>();"
                   "    %2%"
                   "    return ret;"
                   "}") %
                    tuple_type_desc->name % function_body.str();
}

void GenerateValueToTuple(Context *const context,
                          const dag::type::Tuple *const tuple_type) {
    struct ValueToTupleVisitor
        : public Visitor<ValueToTupleVisitor, const dag::type::FieldType,
                         boost::mpl::list<                //
                                 const dag::type::Array,  //
                                 const dag::type::Atomic  //
                                 >::type,
                         std::string> {
        ValueToTupleVisitor(
                // NOLINTNEXTLINE(modernize-pass-by-value)
                const std::string &input_var_name, Context *const context)
            : input_var_name_(input_var_name), context_(context) {}

        auto operator()(const dag::type::Array *const type) const
                -> std::string {
            const auto *const inner_type = type->tuple_type;
            auto *const inner_typedef =
                    context_->tuple_type_descs().at(inner_type).get();

            return (format("{%1%->as<Array>()->data.as<%2%>(),"
                           " %1%->as<Array>()->outer_shape[0],"
                           " %1%->as<Array>()->offsets[0],"
                           " %1%->as<Array>()->shape[0]}") %
                    input_var_name_ % inner_typedef->name)
                    .str();
        }

        auto operator()(const dag::type::Atomic *const type) const
                -> std::string {
            return (format("%1%->as<%2%>()->value") % input_var_name_ %
                    AtomicTypeNameToRuntimeTypename(type->type))
                    .str();
        }

        auto operator()(const dag::type::FieldType *const type) const
                -> std::string {
            throw std::runtime_error("Unknown field type: " +
                                     type->to_string());
        }

        std::string input_var_name_;
        Context *const context_;
    };

    auto *const tuple_type_desc =
            context->tuple_type_descs().at(tuple_type).get();

    if (tuple_type_desc->has_value_to_struct) return;
    tuple_type_desc->has_value_to_struct = true;

    ValueToTupleVisitor visitor("", context);
    std::vector<std::string> field_values;
    for (size_t i = 0; i < tuple_type->field_types.size(); i++) {
        auto const &type = tuple_type->field_types[i];
        visitor.input_var_name_ =
                "v->as<Tuple>()->fields[" + std::to_string(i) + "]";
        field_values.emplace_back(visitor.Visit(type));
    }

    context->definitions() <<  //
            format("template <>"
                   "Optional<%1%> ValueToTuple("
                   "        const std::shared_ptr<Value> &v) {"
                   "    if (dynamic_cast<None*>(v.get()) != nullptr) {"
                   "        return {};"
                   "    }"
                   "    return %1%{%2%};"
                   "}") %
                    tuple_type_desc->name % join(field_values, ", ");
}

auto GenerateExecuteTuples(DAG *const dag, Context *const context)
        -> FunctionDef {
    std::stringstream plan_body;

    CodeGenVisitor visitor(dag, context, plan_body);
    dag::utils::ApplyInReverseTopologicalOrder(dag, visitor.functor());

    // Compute execute function parameters
    std::map<size_t, const DAGOperator *> inputs;
    for (auto const input : dag->inputs()) {
        auto *const op = input.second.op;
        auto const dag_port = input.first;
        assert(input.second.port == 0);

        auto const ret = inputs.emplace(dag_port, op);
        if (!ret.second) {
            assert(ret.first->second->tuple->type == op->tuple->type);
        }
    }

    std::vector<std::string> packed_input_args;
    for (auto const input : inputs) {
        const auto *const op = input.second;
        auto const dag_port = input.first;

        const std::string param_num = std::to_string(dag_port);
        const auto *const tuple_type = op->tuple->type;

        // Parameters for function signature of 'execute_tuples'
        auto *const tuple_typedef =
                context->tuple_type_descs().at(tuple_type).get();
        packed_input_args.emplace_back((format("Optional<%1%> input_%2%") %
                                        tuple_typedef->name % param_num)
                                               .str());
    }

    auto const func_name = context->GenerateSymbolName("execute_tuples", true);
    auto sink = visitor.operator_descs_[dag->output().op];
    context->definitions() <<  //
            format("Optional<%1%> %5%(%2%) {"
                   "    %3%\n"
                   "    /** collecting the result **/"
                   "    %4%.open();"
                   "    auto result = %4%.next();"
                   "    %4%.close();"
                   "    return result;"
                   "}") %
                    sink.return_type->name % join(packed_input_args, ", ") %
                    plan_body.str() % sink.var_name % func_name;

    return {func_name, sink.return_type};
}

auto GenerateExecuteValues(DAG *const dag, Context *const context)
        -> std::string {
    // Generate plan function on tuples
    auto execute_tuples = GenerateExecuteTuples(dag, context);

    // Includes needed for generate_values
    context->includes().emplace("\"runtime/jit/memory/shared_pointer.hpp\"");
    context->includes().emplace("\"runtime/jit/values/array.hpp\"");
    context->includes().emplace("\"runtime/jit/values/atomics.hpp\"");
    context->includes().emplace("\"runtime/jit/values/json_parsing.hpp\"");
    context->includes().emplace("\"runtime/jit/values/none.hpp\"");
    context->includes().emplace("\"runtime/jit/values/tuple.hpp\"");
    context->includes().emplace("\"runtime/jit/values/value.hpp\"");
    context->includes().emplace("\"runtime/jit/operators/optional.hpp\"");
    context->includes().emplace("<omp.h>");

    context->declarations() << "using namespace runtime::values;" << std::endl;

    // Compute execute function parameters
    std::vector<std::string> pack_input_args;
    for (auto const input : dag->inputs()) {
        auto *const op = input.second.op;
        auto const dag_port = input.first;

        GenerateValueToTuple(context, op->tuple->type);

        auto *const tuple_type_desc =
                context->tuple_type_descs().at(op->tuple->type).get();

        pack_input_args.emplace_back((format("ValueToTuple<%1%>(inputs[%2%])") %
                                      tuple_type_desc->name % dag_port)
                                             .str());
    }

    // Main executable file: plan function on runtime values
    const auto *const return_type = dag->output().op->tuple->type;
    const auto func_name = context->GenerateSymbolName("execute_values", true);
    GenerateTupleToValue(context, return_type);

    context->definitions() <<  //
            format("VectorOfValues %2%(const VectorOfValues &inputs)"
                   "{"
                   "    return {TupleToValue(%3%(%1%))};"
                   "}") %
                    join(pack_input_args, ", ") % func_name %
                    execute_tuples.name;

    return func_name;
}

auto GenerateExecutePipelines(Context *const context, DAG *const dag)
        -> std::string {
    using ResultNames = std::map<const DAGOperator *, std::string>;

    struct GeneratePipelinesVisitor
        : public Visitor<GeneratePipelinesVisitor, const DAGOperator,
                         boost::mpl::list<            //
                                 DAGParameterLookup,  //
                                 DAGPipeline          //
                                 >::type,
                         std::string> {
        GeneratePipelinesVisitor(const DAG *const dag, Context *const context,
                                 ResultNames *const result_names)
            : dag_(dag), context_(context), result_names_(result_names) {}

        auto operator()(const DAGParameterLookup *const op) -> std::string {
            auto const result_name = GenerateResultVarName(op);
            return (format("VectorOfValues %1% = { inputs.at(%2%) };") %  //
                    result_name % dag_->input_port(op))
                    .str();
        }

        auto operator()(const DAGPipeline *const op) -> std::string {
            auto const result_name = GenerateResultVarName(op);

            std::vector<std::string> input_names(dag_->in_degree(op));
            std::set<std::string> input_deps;
            for (auto const f : dag_->in_flows(op)) {
                auto const input_name = result_names_->at(f.source.op);
                input_names[f.target.port] =
                        (format("%1%[%2%]") % input_name % f.source.port).str();
                input_deps.emplace(input_name);
            }

            // Call nested code gen
            auto inner_plan =
                    GenerateExecuteValues(dag_->inner_dag(op), context_);

            return (format("VectorOfValues %1%;"
                           "\n#pragma omp task default(shared)"
                           "        depend(out:%1%) depend(in: %4%)\n{"
                           "    %1% = %2%({%3%});"
                           "}") %
                    result_name % inner_plan % join(input_names, ",") %
                    boost::join(input_deps, ", "))
                    .str();
        }

        auto operator()(const DAGOperator *const op) -> std::string {
            throw std::runtime_error(
                    "Pipeline generation encountered unknown operator type: " +
                    op->name());
        }

    private:
        auto GenerateResultVarName(const DAGOperator *const op) -> std::string {
            auto const name = context_->GenerateSymbolName(
                    (format("op_%1%_result") % op->id).str(), true);
            auto const ret = result_names_->emplace(op, name);
            assert(ret.second);
            return name;
        }

        const DAG *const dag_;
        Context *const context_;
        ResultNames *const result_names_;
    };

    // Generate pipeline-level plan body
    std::stringstream plan_body;
    ResultNames result_names;

    dag::utils::ApplyInReverseTopologicalOrder(
            dag, [&](const DAGOperator *const op) {
                GeneratePipelinesVisitor visitor(dag, context, &result_names);
                plan_body << visitor.Visit(op);
            });

    // Generate glue code
    const auto func_name =
            context->GenerateSymbolName("execute_pipelines", true);
    const auto sink_result_name = result_names[dag->output().op];

    context->definitions() <<  //
            format("extern \"C\" {"
                   "VectorOfValues %1%(const VectorOfValues &inputs) {"
                   "    VectorOfValues result;\n"
                   "    #pragma omp parallel shared(result)\n"
                   "    #pragma omp single\n"
                   "    {"
                   "        %2%\n"
                   "        #pragma omp taskwait\n"
                   "        result = %3%;"
                   "    }"
                   "    return std::move(result);"
                   "}}") %
                    func_name % plan_body.str() % sink_result_name;

    return func_name;
}

auto GenerateCppFunctor(Context *context, const std::string &func_name_prefix,
                        const std::string &callee_func_name,
                        const std::vector<const StructDef *> &input_types)
        -> std::string {
    // Generate symbol names
    const auto class_name =
            context->GenerateSymbolName(func_name_prefix + "_functor");

    // Prepare functor defintion
    std::vector<std::string> input_args;
    std::vector<std::string> conversion_statements;
    std::vector<std::string> call_args;
    for (size_t i = 0; i < input_types.size(); i++) {
        auto const &input_type = input_types.at(i);

        // Each input tuple is one argument of the functor
        const auto input_var_name = "t" + std::to_string(i);
        input_args.emplace_back(input_type->name + " " + input_var_name);

        // Statement for converting to std::tuple
        const auto statement =
                (boost::format("auto const st%1% = TupleToStdTuple(t%1%);") % i)
                        .str();
        conversion_statements.emplace_back(statement);

        // Usage of this std::tuple for function call
        call_args.emplace_back("st" + std::to_string(i));
    }

    // Emit functor definition
    context->definitions() <<  //
            format("class %s {"
                   "public:"
                   "    auto operator()(%s) {"
                   "        %s"
                   "        auto const res = %s(%s);"
                   "        return StdTupleToTuple(res);"
                   "    }"
                   "};") %
                    class_name % join(input_args, ",") %
                    join(conversion_statements, "") % callee_func_name %
                    join(call_args, ",");

    return class_name;
}

auto GenerateLlvmFunctor(Context *const context,
                         const std::string &func_name_prefix,
                         const std::string &llvm_ir,
                         const std::vector<const StructDef *> &input_types,
                         const std::string &return_type) -> std::string {
    // Generate symbol names
    const auto func_name =
            context->GenerateSymbolName(func_name_prefix + "_llvm");
    const auto class_name =
            context->GenerateSymbolName(func_name_prefix + "_functor");

    // Emit LLVM code
    StoreLlvmCode(context, llvm_ir, func_name);

    // Prepare functor defintion
    std::vector<std::string> input_args;
    std::vector<std::string> call_args;
    std::vector<std::string> call_types;
    for (auto const &input_type : input_types) {
        // Each input tuple is one argument of the functor
        const auto input_var_name = "t" + std::to_string(input_args.size());
        input_args.emplace_back(input_type->name + " " + input_var_name);

        // Each collection of all input tuples is one argument to the function
        // --> construct values for the call
        for (auto const &field : input_type->names) {
            // NOLINTNEXTLINE(performance-inefficient-string-concatenation)
            call_args.emplace_back(input_var_name + "." + field);
        }

        // --> construct types for the declaration
        call_types.insert(call_types.begin(), input_type->types.begin(),
                          input_type->types.end());
    }

    // Emit functor definition
    context->definitions() <<  //
            format("class %s {"
                   "public:"
                   "    auto operator()(%s) {"
                   "        %s res;"
                   "        %s(&res, %s);"
                   "        return res;"
                   "    }"
                   "};") %
                    class_name % join(input_args, ",") % return_type %
                    func_name % join(call_args, ",");

    // Emit function declaration
    context->declarations() << format("extern \"C\" { void %s(%s*, %s); }") %
                                       func_name % return_type %
                                       join(call_types, ",");

    return class_name;
}

void StoreLlvmCode(Context *const context, const std::string &llvm_ir,
                   const std::string &func_name) {
    llvm_helpers::Function func(llvm_ir);
    func.AdjustLinkage();

    // Replace the function name with ours
    auto patched_ir = boost::replace_all_copy(
            llvm_ir, llvm_helpers::Function::kEntryFunctionName, func_name);

    // Replace all related symbols with a unique name
    boost::replace_all(patched_ir, llvm_helpers::Function::kNotUniqueName,
                       func_name + "_helper");

    // Write code
    boost::filesystem::ofstream file(
            context->GenerateLlvmCodeFileName(func_name));
    file << patched_ir;
}

auto EmitStructDefinition(Context *const context, const dag::type::Type *key,
                          const std::vector<std::string> &types,
                          const std::vector<std::string> &names)
        -> std::pair<const StructDef *, bool> {
    if (context->tuple_type_descs().count(key) > 0) {
        return std::make_pair(context->tuple_type_descs().at(key).get(), false);
    }

    auto tuple_typedef = std::make_unique<StructDef>(
            context->GenerateSymbolName("tuple"), types, names);
    context->declarations() << tuple_typedef->ComputeDefinition();

    const auto ret =
            context->tuple_type_descs().emplace(key, std::move(tuple_typedef));
    assert(ret.second);

    return std::make_pair(ret.first->second.get(), true);
}

auto EmitTupleStructDefinition(Context *const context,
                               const dag::type::Tuple *tuple)
        -> const StructDef * {
    class FieldTypeVisitor
        : public Visitor<FieldTypeVisitor, const dag::type::FieldType,
                         boost::mpl::list<const dag::type::Atomic,
                                          const dag::type::Array>::type,
                         std::string> {
    public:
        explicit FieldTypeVisitor(Context *const context) : context_(context) {}

        auto operator()(const dag::type::Atomic *const field) -> std::string {
            return field->type;
        }

        auto operator()(const dag::type::Array *const field) -> std::string {
            const auto *item_desc =
                    EmitTupleStructDefinition(context_, field->tuple_type);
            std::vector<std::string> names;
            std::vector<std::string> types;
            names.emplace_back("data");
            names.emplace_back(boost::str(format("outer_shape [%s]") %
                                          field->num_dimensions));
            names.emplace_back(
                    boost::str(format("offsets [%s]") % field->num_dimensions));
            names.emplace_back(
                    boost::str(format("shape [%s]") % field->num_dimensions));
            types.emplace_back(
                    boost::str(format("SharedPointer<%s>") % item_desc->name));
            types.emplace_back("size_t");
            types.emplace_back("size_t");
            types.emplace_back("size_t");
            return EmitStructDefinition(context_, field, types, names)
                    .first->name;
        }

        auto operator()(const dag::type::FieldType *const field) const
                -> std::string {
            throw std::runtime_error(
                    "Code gen encountered unknown field type: " +
                    field->to_string());
        }

    private:
        Context *const context_;
    };

    // Visitor for struct definitions for nested types
    FieldTypeVisitor visitor(context);

    // Compute and emit struct definition
    std::vector<std::string> types;
    std::vector<std::string> names;
    for (auto pos = 0; pos < tuple->field_types.size(); pos++) {
        const auto *f = tuple->field_types[pos];
        auto item_type = visitor.Visit(f);
        types.emplace_back(item_type);
        names.emplace_back("v" + std::to_string(pos));
    }
    auto const ret = EmitStructDefinition(context, tuple, types, names);

    // This call created the struct definition, so we also create the helpers
    if (ret.second) {
        std::vector<std::string> from_tuple_statements;
        std::vector<std::string> field_expressions;
        for (auto i = 0; i < names.size(); i++) {
            from_tuple_statements.emplace_back(
                    (format("ret.%1% = std::get<%2%>(t);") % names[i] % i)
                            .str());
            field_expressions.emplace_back("t." + names[i]);
        }

        context->declarations() <<  //
                (format("template<>"
                        "auto StdTupleToTuple<std::tuple<%2%>>("
                        "        const std::tuple<%2%> &t) {"
                        "    %1% ret;"
                        "    %3%"
                        "    return ret;"
                        "}"
                        "template<>"
                        "auto TupleToStdTuple<%1%>(const %1% &t) {"
                        "    return std::make_tuple(%4%);"
                        "}") %
                 ret.first->name % join(types, ", ") %
                 join(from_tuple_statements, "") % join(field_expressions, ","))
                        .str();
    }

    return ret.first;
}

}  // namespace code_gen::cpp
