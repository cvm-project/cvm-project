#ifndef CODE_GEN_CODE_GEN_HPP
#define CODE_GEN_CODE_GEN_HPP

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

void GenerateTupleToValue(Context *context, const dag::type::Tuple *tuple_type);

void GenerateValueToTuple(Context *context, const dag::type::Tuple *tuple_type);

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

#endif  // CODE_GEN_CODE_GEN_HPP
