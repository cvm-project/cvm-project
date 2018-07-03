#include "context.hpp"

#include <cassert>
#include <string>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

using boost::algorithm::join;
using boost::format;

namespace code_gen {
namespace cpp {

StructDef::StructDef(
        const std::string &name,  // NOLINT modernize-pass-by-value
                                  // NOLINTNEXTLINE modernize-pass-by-value
        const std::vector<std::string> &types,
        // NOLINTNEXTLINE modernize-pass-by-value
        const std::vector<std::string> &names)
    : name(name), types(types), names(names) {
    assert(types.size() == names.size());
}

std::string StructDef::ComputeDefinition() const {
    std::vector<std::string> name_types;
    name_types.reserve(names.size());
    for (auto i = 0; i < names.size(); i++) {
        name_types.emplace_back(types[i] + " " + names[i]);
    }
    return (format("typedef struct { %s; } %s;") % join(name_types, "; ") %
            name)
            .str();
}

std::string Context::GenerateSymbolName(const std::string &prefix,
                                        const bool try_empty_suffix) {
    const size_t counter = ((*unique_counters_)[prefix])++;
    if (counter == 0 && try_empty_suffix) {
        return prefix;
    }
    return prefix + "_" + std::to_string(counter);
}

}  // namespace cpp
}  // namespace code_gen
