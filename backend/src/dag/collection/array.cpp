//
// Created by sabir on 22.05.18.
//

#include <memory>

#include <json.hpp>

#include "array.hpp"
#include "tuple.hpp"

namespace dag {
namespace collection {

Array::Array(const type::FieldType *field_type_, const size_t &position,
             const nlohmann::json &json)
    : FieldBase(field_type_, position),
      tuple_(std::make_unique<Tuple>(json.at("tuple_type"))) {}

}  // namespace collection
}  // namespace dag
