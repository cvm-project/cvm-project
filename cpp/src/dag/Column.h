//
// Created by sabir on 26.08.17.
//

#ifndef CPP_COLUMN_H
#define CPP_COLUMN_H

#include <iostream>
#include <vector>

#include "dag/TupleField.h"

/**
 * part of the schema
 */
class Column {
public:
    static Column *makeColumn();

    void addField(TupleField *field);

    void addFields(const vector<TupleField *> &fields);

    bool operator==(const Column &other) { return id == other.id; }

    bool operator<(const Column &other) { return id < other.id; }

    std::string get_name() { return "c" + to_string(id); }

    static void delete_columns();

    vector<TupleField *> getFields();

private:
    Column() { id = column_counter++; }

    size_t id;
    vector<TupleField *> fields;
    static size_t column_counter;
    static vector<Column *> all_columns;
};

#endif  // CPP_COLUMN_H
