//
// Created by sabir on 04.07.17.
//

#ifndef CPP_COLLECTIONSOURCEOPERATOR_H
#define CPP_COLLECTIONSOURCEOPERATOR_H


#include <iostream>
#include "Operator.h"
#include "vector"
#include "dag/DAGCollection.h"

template<class Tuple>
class CollectionSourceOperator : public Operator {
public:
    void printName() {
        std::cout << "collection op\n";
    }

    void open() {
        it = values.begin();
    }

    Optional<Tuple> next() {
        if (it != values.end()) {
            auto r = *it;
            it++;
            return r;
        }
        return {};
    }

    std::vector<Tuple> values;
private:
    typename std::vector<Tuple>::iterator it;
};

template<class Tuple>
CollectionSourceOperator<Tuple> makeCollectionSourceOperator() {
    return CollectionSourceOperator<Tuple>();
};

#endif //CPP_COLLECTIONSOURCEOPERATOR_H
