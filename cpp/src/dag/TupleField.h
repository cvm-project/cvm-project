//
// Created by sabir on 26.08.17.
//

#ifndef CPP_TUPLEFIELD_H
#define CPP_TUPLEFIELD_H

#include <string>

using namespace std;

class Column;

class TupleField {
public:

    TupleField(string type, size_t pos) : type(type), position(pos) {}

    string type;
    Column *column = NULL;
    size_t position;

    const bool operator<(const TupleField &other) const {
        return position < other.position;
    }
};


#endif //CPP_TUPLEFIELD_H
