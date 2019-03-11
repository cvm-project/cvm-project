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

    auto const Register = ValueParserRegistry::Register;

    Register("array", std::make_unique<DefaultValueParser<Array>>());
    Register("tuple", std::make_unique<DefaultValueParser<Tuple>>());
    Register("float", std::make_unique<DefaultValueParser<Float>>());
    Register("double", std::make_unique<DefaultValueParser<Double>>());
    Register("int", std::make_unique<DefaultValueParser<Int32>>());
    Register("long", std::make_unique<DefaultValueParser<Int64>>());
    Register("bool", std::make_unique<DefaultValueParser<Bool>>());
    Register("none", std::make_unique<DefaultValueParser<None>>());
    Register("std::string", std::make_unique<DefaultValueParser<String>>());

    has_loaded = true;
}

}  // namespace values
}  // namespace runtime
