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
            // cppcheck-suppress passedByValue
            std::string llvm_code_dir,
            std::vector<std::string> *const llvm_code_files,
            std::unordered_map<std::string, size_t> *const unique_counters,
            std::set<std::string> *const includes,
            TupleTypeRegistry *const tuple_type_descs)
        : declarations_(declarations),
          definitions_(definitions),
          llvm_code_dir_(std::move(llvm_code_dir)),
          llvm_code_files_(llvm_code_files),
          unique_counters_(unique_counters),
          includes_(includes),
          tuple_type_descs_(tuple_type_descs) {}

    auto GenerateSymbolName(const std::string &prefix,
                            bool try_empty_suffix = false) -> std::string;
    auto GenerateLlvmCodeFileName(const std::string &prefix) -> std::string {
        llvm_code_files_->emplace_back(llvm_code_dir_ + "/" +
                                       GenerateSymbolName(prefix, true) +
                                       ".ll");
        return llvm_code_files_->back();
    }

    auto declarations() -> std::ostream & { return *declarations_; }
    auto definitions() -> std::ostream & { return *definitions_; }
    [[nodiscard]] auto llvm_code_files() const -> auto & {
        return llvm_code_files_;
    }

    auto includes() -> std::set<std::string> & { return *includes_; }
    auto tuple_type_descs() -> TupleTypeRegistry & {
        return *tuple_type_descs_;
    }

private:
    std::ostream *const declarations_;
    std::ostream *const definitions_;
    const std::string llvm_code_dir_;
    std::vector<std::string> *const llvm_code_files_;
    std::unordered_map<std::string, size_t> *const unique_counters_;
    std::set<std::string> *const includes_;
    TupleTypeRegistry *const tuple_type_descs_;
};

}  // namespace cpp
}  // namespace code_gen

#endif  // CODE_GEN_CONTEXT_HPP
