//
// Created by sabir on 26.08.17.
//

#include "Column.h"

vector<Column *> Column::all_columns;

void Column::addField(TupleField *field) {
    fields.push_back(field);
    field->column = this;
}

void Column::addFields(const vector<TupleField *> &fields) {
    for (auto field : fields) {
        addField(field);
    }
}

size_t Column::column_counter = 0;

Column *Column::makeColumn() {
    Column *c = new Column();
    Column::all_columns.push_back(c);
    return c;
}

void Column::delete_columns() {
    for (auto c : Column::all_columns) {
        delete (c);
    }
}

vector<TupleField *> Column::getFields() {
    return fields;
}
