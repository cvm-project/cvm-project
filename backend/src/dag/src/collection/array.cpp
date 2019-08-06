#include "dag/collection/array.hpp"

#include <memory>

#include <nlohmann/json.hpp>

#include "dag/collection/tuple.hpp"

namespace dag {
namespace collection {

Array::Array(const type::FieldType *field_type_, const size_t &position,
             const nlohmann::json &json)
    : FieldBase(field_type_, position),
      tuple_(std::make_unique<Tuple>(json.at("tuple_type"))) {}

}  // namespace collection
}  // namespace dag
