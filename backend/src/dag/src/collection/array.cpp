#include "dag/collection/array.hpp"

#include <memory>

#include "dag/collection/tuple.hpp"

namespace dag {
namespace collection {

Array::Array(const type::Array *const array_type, const size_t &position)
    : FieldBase(array_type, position),
      tuple_(std::make_unique<Tuple>(array_type->tuple_type)) {}

}  // namespace collection
}  // namespace dag
