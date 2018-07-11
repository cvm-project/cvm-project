#ifndef CODE_GEN_CPP_CODE_GEN_HPP
#define CODE_GEN_CPP_CODE_GEN_HPP

#include <string>
#include <vector>

#include "context.hpp"
#include "dag/DAG.h"
#include "dag/type/tuple.hpp"

namespace code_gen {
namespace cpp {

std::string AtomicTypeNameToRuntimeTypename(const std::string &type_name);

std::string ComputeStructToValue(std::ostream &output, Context *context,
                                 const std::string &input_var_name,
                                 const dag::type::Tuple *tuple_type);

std::string ComputeValueToStruct(const std::string &input_var_name,
                                 const dag::type::Tuple *tuple_type,
                                 Context *context);

FunctionDef GenerateExecuteTuples(DAG *dag, Context *context);

std::string GenerateExecuteValues(DAG *dag, Context *context);

std::string GenerateLlvmFunctor(
        Context *context, const std::string &func_name_prefix,
        const std::string &llvm_ir,
        const std::vector<const StructDef *> &input_types,
        const std::string &return_type);

void StoreLlvmCode(Context *context, const std::string &llvm_ir,
                   const std::string &func_name);

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_CODE_GEN_HPP
