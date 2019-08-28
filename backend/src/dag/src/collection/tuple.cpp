#include "dag/collection/tuple.hpp"

#include <memory>

#include "dag/collection/array.hpp"
#include "dag/collection/atomic.hpp"
#include "dag/collection/attribute_id.hpp"
#include "utils/raw_ptr.hpp"

namespace dag {
namespace collection {

Tuple::Tuple(const type::Tuple *type) {
    this->type = type;
    for (size_t i = 0; i < type->field_types.size(); i++) {
        auto const field_type = type->field_types.at(i);

        auto const array_type = dynamic_cast<const type::Array *>(field_type);
        if (array_type != nullptr) {
            this->fields.push_back(std::make_unique<Array>(array_type, i));
        } else {
            this->fields.push_back(std::make_unique<Atomic>(field_type, i));
        }
        Field *const field = this->fields.back().get();
        const auto attribute_id = AttributeId::MakeAttributeId();
        attribute_id->AddField(field);
    }
}

}  // namespace collection
}  // namespace dag
