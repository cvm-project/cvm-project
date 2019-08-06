#include "attribute_id.hpp"

namespace dag {
namespace collection {

Field::~Field() {
    if (attribute_id_ != nullptr) {
        attribute_id_->RemoveField(this);
    }
}

size_t Field::position() const { return position_; }

void Field::set_position(size_t position) {
    assert(position >= 0);
    position_ = position;
}

const dag::type::FieldType *Field::field_type() const { return field_type_; }

void Field::set_field_type(const dag::type::FieldType *field_type) {
    field_type_ = field_type;
}

const std::shared_ptr<dag::AttributeId> &Field::attribute_id() const {
    return attribute_id_;
}

void Field::AddProperty(const FieldProperty &fieldProperty) {
    properties_.insert(fieldProperty);
}

void Field::RemoveProperty(const FieldProperty &fieldProperty) {
    properties_.erase(fieldProperty);
}

const std::set<FieldProperty> &Field::properties() const { return properties_; }

void Field::CopyProperties(const Field &other) {
    for (auto p : other.properties()) {
        AddProperty(p);
    }
}

void Field::ClearProperties() { properties_.clear(); }

}  // namespace collection
}  // namespace dag
