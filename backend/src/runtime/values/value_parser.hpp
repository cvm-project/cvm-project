#ifndef RUNTIME_VALUES_VALUE_PARSER_HPP
#define RUNTIME_VALUES_VALUE_PARSER_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/range/iterator_range.hpp>
#include <json.hpp>

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

class ValueParserRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<const ValueParser>>
            parsers_;

public:
    using ParserRange =
            boost::iterator_range<decltype(parsers_)::const_iterator>;

    static bool RegisterParser(const std::string &type_name,
                               const ValueParser *parser);
    static const ValueParser *parser(const std::string &type_name);
    static ParserRange parsers();

private:
    static ValueParserRegistry *instance();
};

void LoadValueParsers();

}  // namespace values
}  // namespace runtime

#endif  // RUNTIME_VALUES_VALUE_PARSER_HPP
