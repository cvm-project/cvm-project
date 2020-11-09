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

auto GenerateCode(DAG *dag, const std::string &config)
        -> std::pair<std::string, std::string>;

auto AtomicTypeNameToRuntimeTypename(const std::string &type_name)
        -> std::string;

void GenerateTupleToValue(Context *context, const dag::type::Tuple *tuple_type);

void GenerateValueToTuple(Context *context, const dag::type::Tuple *tuple_type);

auto GenerateExecuteTuples(DAG *dag, Context *context) -> FunctionDef;

auto GenerateExecuteValues(DAG *dag, Context *context) -> std::string;

auto GenerateExecutePipelines(Context *context, DAG *dag) -> std::string;

auto GenerateCppFunctor(Context *context, const std::string &func_name_prefix,
                        const std::string &callee_func_name,
                        const std::vector<const StructDef *> &input_types)
        -> std::string;

auto GenerateLlvmFunctor(Context *context, const std::string &func_name_prefix,
                         const std::string &llvm_ir,
                         const std::vector<const StructDef *> &input_types,
                         const std::string &return_type) -> std::string;

void StoreLlvmCode(Context *context, const std::string &llvm_ir,
                   const std::string &func_name);

auto EmitStructDefinition(Context *context, const dag::type::Type *key,
                          const std::vector<std::string> &types,
                          const std::vector<std::string> &names)
        -> std::pair<const StructDef *, bool>;

auto EmitTupleStructDefinition(Context *context, const dag::type::Tuple *tuple)
        -> const StructDef *;

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CODE_GEN_HPP
