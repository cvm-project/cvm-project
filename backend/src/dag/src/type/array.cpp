#include "dag/type/array.hpp"

#include <memory>

#include <boost/format.hpp>
#include <boost/polymorphic_pointer_cast.hpp>

#include <nlohmann/json.hpp>

#include "dag/type/tuple.hpp"
#include "utils/raw_ptr.hpp"

using dag::type::Array;
using dag::type::ArrayLayout;
using dag::type::Tuple;

namespace dag {
namespace type {

const Array *Array::MakeArray(const dag::type::Tuple *type,
                              const ArrayLayout &layout,
                              const size_t &num_dimensions) {
    std::unique_ptr<Array> ret(new Array(type));
    ret->layout = layout;
    ret->num_dimensions = num_dimensions;
    return boost::polymorphic_pointer_downcast<const Array>(
            MakeType(std::move(ret)));
}

std::string Array::to_string() const {
    return "{" + tuple_type->to_string() + "}" + dag::type::to_string(layout) +
           std::to_string(num_dimensions);
}

void Array::from_json(const nlohmann::json &json) {
    assert(json.at("type").get<std::string>() == std::string("array"));
    layout = json.at("layout");
    num_dimensions = json.at("num_dimensions");
    tuple_type = json.at("tuple_type").get<raw_ptr<const Tuple>>().get();
}

void Array::to_json(nlohmann::json *json) const {
    json->emplace("type", "array");
    json->emplace("layout", layout);
    json->emplace("num_dimensions", num_dimensions);
    json->emplace("tuple_type", tuple_type);
}

std::string to_string(const ArrayLayout &layout) {
    switch (layout) {
        case kC:
            return "C";
        case kF:
            return "F";
        default:
            return "C";
    }
}

}  // namespace type
}  // namespace dag

// JSON serialization ArrayLayout
ArrayLayout nlohmann::adl_serializer<ArrayLayout>::from_json(
        const nlohmann::json &json) {
    std::string layout = json;
    if (layout == "C") {
        return ArrayLayout::kC;
    }
    if (layout == "F") {
        return ArrayLayout::kF;
    }
    throw std::runtime_error(
            (boost::format("Unknown array layout %s") % layout).str());
}

void nlohmann::adl_serializer<ArrayLayout>::to_json(nlohmann::json &json,
                                                    ArrayLayout layout) {
    json = dag::type::to_string(layout);
}