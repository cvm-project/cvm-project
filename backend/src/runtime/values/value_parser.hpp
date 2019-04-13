#ifndef RUNTIME_VALUES_VALUE_PARSER_HPP
#define RUNTIME_VALUES_VALUE_PARSER_HPP

#include <memory>

#include <nlohmann/json.hpp>

#include "utils/registry.hpp"
#include "value.hpp"

namespace runtime {
namespace values {

struct ValueParser {
    virtual ~ValueParser() = default;
    // NOLINTNEXTLINE google-runtime-references
    virtual bool try_to_json(nlohmann::json &json, const Value *val) const = 0;
    virtual std::unique_ptr<Value> from_json(
            const nlohmann::json &json) const = 0;
};

using ValueParserRegistry = utils::Registry<const ValueParser>;

void LoadValueParsers();

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_VALUE_PARSER_HPP
