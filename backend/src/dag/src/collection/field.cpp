#include "dag/collection/attribute_id.hpp"

namespace dag {
namespace collection {

Field::~Field() {
    if (attribute_id_ != nullptr) {
        attribute_id_->RemoveField(this);
    }
}

auto Field::position() const -> size_t { return position_; }

void Field::set_position(size_t position) { position_ = position; }

auto Field::field_type() const -> const dag::type::FieldType * {
    return field_type_;
}

void Field::set_field_type(const dag::type::FieldType *field_type) {
    field_type_ = field_type;
}

auto Field::attribute_id() const -> const std::shared_ptr<dag::AttributeId> & {
    return attribute_id_;
}

void Field::AddProperty(const FieldProperty &fieldProperty) {
    properties_.insert(fieldProperty);
}

void Field::RemoveProperty(const FieldProperty &fieldProperty) {
    properties_.erase(fieldProperty);
}

auto Field::properties() const -> const std::set<FieldProperty> & {
    return properties_;
}

void Field::CopyProperties(const Field &other) {
    for (auto p : other.properties()) {
        AddProperty(p);
    }
}

void Field::ClearProperties() { properties_.clear(); }

}  // namespace collection
}  // namespace dag
