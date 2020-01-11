#ifndef CODE_GEN_CONTEXT_HPP
#define CODE_GEN_CONTEXT_HPP

#include <memory>
#include <ostream>
#include <set>
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
    [[nodiscard]] auto ComputeDefinition() const -> std::string;

    const std::string name;
    const std::vector<std::string> types;
    const std::vector<std::string> names;
    bool has_struct_to_value{};
    bool has_value_to_struct{};
};

struct FunctionDef {
    std::string name;
    const StructDef *const return_type;
};

class Context {
public:
    using TupleTypeRegistry = std::unordered_map<const dag::type::Type *,
                                                 std::unique_ptr<StructDef>>;

    Context(std::ostream *const declarations, std::ostream *const definitions,
            std::ostream *const llvm_code,
            std::unordered_map<std::string, size_t> *const unique_counters,
            std::set<std::string> *const includes,
            TupleTypeRegistry *const tuple_type_descs)
        : declarations_(declarations),
          definitions_(definitions),
          llvm_code_(llvm_code),
          unique_counters_(unique_counters),
          includes_(includes),
          tuple_type_descs_(tuple_type_descs) {}

    auto GenerateSymbolName(const std::string &prefix,
                            bool try_empty_suffix = false) -> std::string;

    auto declarations() -> std::ostream & { return *declarations_; }
    auto definitions() -> std::ostream & { return *definitions_; }
    auto llvm_code() -> std::ostream & { return *llvm_code_; }

    auto includes() -> std::set<std::string> & { return *includes_; }
    auto tuple_type_descs() -> TupleTypeRegistry & {
        return *tuple_type_descs_;
    }

private:
    std::ostream *const declarations_;
    std::ostream *const definitions_;
    std::ostream *const llvm_code_;
    std::unordered_map<std::string, size_t> *const unique_counters_;
    std::set<std::string> *const includes_;
    TupleTypeRegistry *const tuple_type_descs_;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CONTEXT_HPP
