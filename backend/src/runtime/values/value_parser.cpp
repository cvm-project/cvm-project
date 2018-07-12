#include "value_parser.hpp"

#include <stdexcept>

#include <boost/range/iterator_range.hpp>

#include "array.hpp"
#include "array_json.hpp"
#include "atomics.hpp"
#include "atomics_json.hpp"
#include "none.hpp"
#include "none_json.hpp"
#include "tuple.hpp"
#include "tuple_json.hpp"
#include "value.hpp"
#include "value_json.hpp"

namespace runtime {
namespace values {

bool ValueParserRegistry::RegisterParser(const std::string &type_name,
                                         const ValueParser *const parser) {
    return instance()
            ->parsers_
            .emplace(type_name, std::unique_ptr<const ValueParser>(parser))
            .second;
}

const ValueParser *ValueParserRegistry::parser(const std::string &type_name) {
    const auto it = instance()->parsers_.find(type_name);
    if (it != instance()->parsers_.end()) {
        return it->second.get();
    }
    throw std::runtime_error("Unknown value of type " + type_name);
}

auto ValueParserRegistry::parsers() -> ParserRange {
    return boost::make_iterator_range(instance()->parsers_.begin(),
                                      instance()->parsers_.end());
}

ValueParserRegistry *ValueParserRegistry::instance() {
    static auto registry = std::make_unique<ValueParserRegistry>();
    return registry.get();
}

template <typename ConcreteValueType>
struct DefaultValueParser : public ValueParser {
    bool try_to_json(nlohmann::json &json,
                     const Value *const val) const override {
        if (dynamic_cast<const ConcreteValueType *>(val) != nullptr) {
            to_json(json, val->as<const ConcreteValueType>());
            return true;
        }
        return false;
    }

    std::unique_ptr<Value> from_json(
            const nlohmann::json &json) const override {
        auto ptr = std::make_unique<ConcreteValueType>();
        runtime::values::from_json(json, ptr.get());
        return ptr;
    }
};

void LoadValueParsers() {
    static bool has_loaded = false;
    if (has_loaded) return;

    auto const Register = ValueParserRegistry::RegisterParser;

    Register("array", new DefaultValueParser<Array>());
    Register("tuple", new DefaultValueParser<Tuple>());
    Register("float", new DefaultValueParser<Float>());
    Register("double", new DefaultValueParser<Double>());
    Register("int", new DefaultValueParser<Int32>());
    Register("long", new DefaultValueParser<Int64>());
    Register("bool", new DefaultValueParser<Bool>());
    Register("none", new DefaultValueParser<None>());
    Register("std::string", new DefaultValueParser<String>());

    has_loaded = true;
}

}  // namespace values
}  // namespace runtime
