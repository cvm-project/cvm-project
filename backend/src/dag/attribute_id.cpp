//
// Created by sabir on 26.08.17.
//

#include "attribute_id.h"

#include <vector>

std::vector<AttributeId *> AttributeId::all_attributes_;

void AttributeId::addField(Field *field) {
    fields_.push_back(field);
    field->attribute_id_ = this;
}

void AttributeId::addFields(const std::vector<Field *> &fields) {
    for (auto field : fields) {
        addField(field);
    }
}

size_t AttributeId::attribute_counter_ = 0;

AttributeId *AttributeId::makeAttributeId() {
    auto *c = new AttributeId();
    AttributeId::all_attributes_.push_back(c);
    return c;
}

void AttributeId::delete_attribute_ids() {
    for (auto c : AttributeId::all_attributes_) {
        delete (c);
    }
    AttributeId::all_attributes_.clear();
}

std::vector<Field *> AttributeId::fields() { return fields_; }
