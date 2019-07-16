#ifndef CODE_GEN_CPP_CODE_GEN_HPP
#define CODE_GEN_CPP_CODE_GEN_HPP

#include <string>
#include <utility>
#include <vector>

class DAG;

namespace dag {
namespace type {

class Tuple;
class Type;

}  // namespace type
}  // namespace dag

namespace code_gen {
namespace cpp {

class Context;
class StructDef;
class FunctionDef;

std::pair<std::string, std::string> GenerateCode(DAG *dag,
                                                 const std::string &config);

std::string AtomicTypeNameToRuntimeTypename(const std::string &type_name);

std::string ComputeStructToValue(std::ostream &output, Context *context,
                                 const std::string &input_var_name,
                                 const dag::type::Tuple *tuple_type);

std::string ComputeValueToStruct(const std::string &input_var_name,
                                 const dag::type::Tuple *tuple_type,
                                 Context *context);

FunctionDef GenerateExecuteTuples(DAG *dag, Context *context);

std::string GenerateExecuteValues(DAG *dag, Context *context);

std::string GenerateExecutePipelines(Context *context, DAG *dag);

std::string GenerateLlvmFunctor(
        Context *context, const std::string &func_name_prefix,
        const std::string &llvm_ir,
        const std::vector<const StructDef *> &input_types,
        const std::string &return_type);

void StoreLlvmCode(Context *context, const std::string &llvm_ir,
                   const std::string &func_name);

std::pair<const StructDef *, bool> EmitStructDefinition(
        Context *context, const dag::type::Type *key,
        const std::vector<std::string> &types,
        const std::vector<std::string> &names);

const StructDef *EmitTupleStructDefinition(Context *context,
                                           const dag::type::Tuple *tuple);

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_CODE_GEN_HPP
