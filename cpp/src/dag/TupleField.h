//
// Created by sabir on 26.08.17.
//

#ifndef CPP_TUPLEFIELD_H
#define CPP_TUPLEFIELD_H

#include <memory>
#include <set>
#include <string>

class Column;

/**
 * Properties specific for each field
 * Field is identified by its position
 */
enum FieldProperty { FL_GROUPED, FL_SORTED, FL_UNIQUE };

class TupleField {
public:
    std::shared_ptr<std::set<FieldProperty>> properties;

    TupleField(const std::string &type, size_t pos)
        : properties(new std::set<FieldProperty>), type(type), position(pos) {}

    std::string type;
    Column *column = NULL;
    size_t position;

    const bool operator<(const TupleField &other) const {
        return position < other.position;
    }
};

#endif  // CPP_TUPLEFIELD_H
