//
// Created by sabir on 26.08.17.
//

#ifndef CPP_TUPLEFIELD_H
#define CPP_TUPLEFIELD_H

#include <string>
#include <set>

using namespace std;

class Column;

/**
 * Properties specific for each field
 * Field is identified by its position
 */
enum FieldProperty {
    FL_GROUPED, FL_SORTED, FL_UNIQUE
};

class TupleField {
public:

    set<FieldProperty> *properties;

    TupleField(string type, size_t pos) : type(type), position(pos) {
        properties = new set<FieldProperty>();
    }

    string type;
    Column *column = NULL;
    size_t position;

    const bool operator<(const TupleField &other) const {
        return position < other.position;
    }

    ~TupleField() {
//        delete (properties);
        properties = NULL;
    }
};


#endif //CPP_TUPLEFIELD_H
