#ifndef CODE_GEN_CPP_CONTEXT_HPP
#define CODE_GEN_CPP_CONTEXT_HPP

#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "dag/type/type.hpp"

namespace code_gen {
namespace cpp {

struct StructDef {
    explicit StructDef(const std::string &name,
                       const std::vector<std::string> &types,
                       const std::vector<std::string> &names);
    std::string ComputeDefinition() const;
    std::string name;
    std::vector<std::string> types;
    std::vector<std::string> names;
};

class Context {
public:
    using TupleTypeRegistry =
            std::unordered_map<const dag::type::Type *, const StructDef *>;

    Context(std::ostream *const plan_tuple_declarations,
            std::ostream *const plan_llvm_declarations,
            std::ostream *const llvm_code,
            std::unordered_map<std::string, size_t> *const unique_counters,
            std::unordered_set<std::string> *const includes,
            TupleTypeRegistry *const tuple_type_descs)
        : plan_tuple_declarations_(plan_tuple_declarations),
          plan_llvm_declarations_(plan_llvm_declarations),
          llvm_code_(llvm_code),
          unique_counters_(unique_counters),
          includes_(includes),
          tuple_type_descs_(tuple_type_descs) {}

    size_t unique_counter(const std::string &name) const {
        return unique_counters_->at(name);
    }

    std::string unique_name(const std::string &name) const {
        return name + "_" + std::to_string(unique_counter(name));
    }

    std::string GenerateLlvmFuncName() {
        IncrementUniqueCounter("_operator_function");
        return unique_name("_operator_function");
    }

    std::string GenerateTupleName() {
        IncrementUniqueCounter("tuple");
        return unique_name("tuple");
    }

    std::string GenerateOperatorName() {
        IncrementUniqueCounter("op");
        return unique_name("op");
    }

    std::ostream &plan_tuple_declarations() {
        return *plan_tuple_declarations_;
    }
    std::ostream &plan_llvm_declarations() { return *plan_llvm_declarations_; }
    std::ostream &llvm_code() { return *llvm_code_; }

    std::unordered_set<std::string> &includes() { return *includes_; }
    TupleTypeRegistry &tuple_type_descs() { return *tuple_type_descs_; }

private:
    void IncrementUniqueCounter(const std::string &name) {
        auto it = unique_counters_->emplace(name, -1).first;
        (it->second)++;
    }

    std::ostream *const plan_tuple_declarations_;
    std::ostream *const plan_llvm_declarations_;
    std::ostream *const llvm_code_;
    std::unordered_map<std::string, size_t> *const unique_counters_;
    std::unordered_set<std::string> *const includes_;
    TupleTypeRegistry *const tuple_type_descs_;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CPP_CONTEXT_HPP
