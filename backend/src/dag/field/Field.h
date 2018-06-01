//
// Created by sabir on 26.08.17.
//

#ifndef DAG_TUPLEFIELD_H
#define DAG_TUPLEFIELD_H

#include <memory>
#include <set>
#include <string>
#include <utility>

class AttributeId;

/**
 * Properties specific for each field
 * Field is identified by its position
 */
enum FieldProperty { FL_GROUPED, FL_SORTED, FL_UNIQUE };

class Field {
public:
    std::shared_ptr<std::set<FieldProperty>> properties;

    // cppcheck-suppress passedByValue
    Field(std::string type, size_t pos)
        : properties(new std::set<FieldProperty>),
          type(std::move(type)),
          position(pos) {}

    std::string type;
    AttributeId *attribute_id_ = nullptr;
    size_t position;

    const bool operator<(const Field &other) const {
        return position < other.position;
    }
};

#endif  // DAG_TUPLEFIELD_H
