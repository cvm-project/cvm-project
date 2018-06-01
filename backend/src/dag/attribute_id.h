//
// Created by sabir on 26.08.17.
//

#ifndef DAG_COLUMN_H
#define DAG_COLUMN_H

#include <iostream>
#include <vector>

#include "dag/field/Field.h"

/**
 * part of the schema
 */
class AttributeId {
public:
    static AttributeId *makeAttributeId();

    void addField(Field *field);

    void addFields(const std::vector<Field *> &fields);

    bool operator==(const AttributeId &other) { return id_ == other.id_; }

    bool operator<(const AttributeId &other) { return id_ < other.id_; }

    std::string name() { return "c" + std::to_string(id_); }

    static void delete_attribute_ids();

    std::vector<Field *> fields();

private:
    AttributeId() { id_ = attribute_counter_++; }

    size_t id_;
    std::vector<Field *> fields_;
    static size_t attribute_counter_;
    static std::vector<AttributeId *> all_attributes_;
};

#endif  // DAG_COLUMN_H
