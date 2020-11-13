#include "dag/collection/attribute_id.hpp"

namespace dag {

size_t AttributeId::column_counter_ = 0;

auto AttributeId::MakeAttributeId() -> std::shared_ptr<AttributeId> {
    return std::shared_ptr<AttributeId>(new AttributeId);
}

void AttributeId::AddField(collection::Field *field) {
    if (field->attribute_id_ != nullptr) {
        field->attribute_id_->RemoveField(field);
    }
    fields_.insert(field);
    field->attribute_id_ = shared_from_this();
}

void AttributeId::RemoveField(collection::Field *field) {
    this->fields_.erase(field);
    field->attribute_id_ = nullptr;
}

void AttributeId::MoveFields(const std::shared_ptr<AttributeId> &other) {
    auto count = other->fields_.size();
    for (const auto &field : other->fields_) {
        fields_.insert(field);
        field->attribute_id_ = shared_from_this();
        // this is needed because reseting the attribute_id to this
        // may incure gc on the other, breaking the iterator
        count--;
        if (count == 0) {
            break;
        }
    }
    other->fields_.clear();
}

}  // namespace dag
