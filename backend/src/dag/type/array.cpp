//
// Created by sabir on 11.05.18.
//

#include "array.hpp"

#include <boost/format.hpp>
#include <boost/polymorphic_pointer_cast.hpp>

#include <json.hpp>

#include "utils/utils.h"

using dag::type::Array;
using dag::type::ArrayLayout;
using std::string;

namespace dag {
namespace type {

const Array *Array::MakeArray(const dag::type::Tuple *type,
                              const ArrayLayout &layout,
                              const size_t &number_dim) {
    auto ar = new Array(type);
    ar->layout = layout;
    ar->number_dim = number_dim;
    return boost::polymorphic_pointer_downcast<const Array>(
            registry()
                    .emplace(ar->to_string(), std::unique_ptr<const Type>(ar))
                    .first->second.get());
}

string Array::to_string() const {
    return "{" + tuple_type->to_string() + "}" + dag::type::to_string(layout) +
           std::to_string(number_dim);
}

void Array::to_json(nlohmann::json *json) const {
    json->emplace("type", "array");
    json->emplace("layout", layout);
    json->emplace("dim", number_dim);
    json->emplace("output_type", make_raw(tuple_type));
}

string to_string(const ArrayLayout &layout) {
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

// JSON serialization

// Array

raw_ptr<const Array> nlohmann::adl_serializer<raw_ptr<const Array>>::from_json(
        const nlohmann::json &json) {
    // tuple
    raw_ptr<const dag::type::Tuple> tuple = json.at("output_type");
    return raw_ptr<const Array>(
            Array::MakeArray(tuple.get(), json.at("layout"), json.at("dim")));
}

// ArrayLayout
ArrayLayout nlohmann::adl_serializer<ArrayLayout>::from_json(
        const nlohmann::json &json) {
    string layout_ = json;
    if (layout_ == "C") {
        return ArrayLayout::kC;
    }
    if (layout_ == "F") {
        return ArrayLayout::kF;
    }
    throw(boost::format("Unknown array layout %s") % layout_).str();
}

void nlohmann::adl_serializer<ArrayLayout>::to_json(nlohmann::json &json,
                                                    ArrayLayout layout) {
    json = dag::type::to_string(layout);
}