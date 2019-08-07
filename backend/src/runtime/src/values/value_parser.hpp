#ifndef VALUES_VALUE_PARSER_HPP
#define VALUES_VALUE_PARSER_HPP

#include <memory>

#include <nlohmann/json.hpp>

#include "runtime/jit/values/value.hpp"
#include "utils/registry.hpp"

namespace runtime {
namespace values {

struct ValueParser {
    virtual ~ValueParser() = default;
    // NOLINTNEXTLINE(google-runtime-references)
    virtual bool try_to_json(nlohmann::json &json, const Value *val) const = 0;
    virtual std::unique_ptr<Value> from_json(
            const nlohmann::json &json) const = 0;
};

using ValueParserRegistry = utils::Registry<const ValueParser>;

void LoadValueParsers();

}  // namespace values
}  // namespace runtime

#endif  // VALUES_VALUE_PARSER_HPP
