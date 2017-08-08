//
// Created by sabir on 04.07.17.
//

#ifndef CPP_JOINOPERATOR_H
#define CPP_JOINOPERATOR_H

#include <iostream>
#include "Operator.h"

template<class Upstream1, class Upstream2, class Tuple>
class JoinOperator : public Operator {
public:
    Upstream1 *upstream1;
    Upstream2 *upstream2;

    JoinOperator(Upstream1 *upstream1, Upstream2 *upstream2) : upstream1(upstream1), upstream2(upstream2) {};

    void printName() {
        std::cout << "join op\n";
        upstream1->printName();
        upstream2->printName();
    }


    void INLINE Open() override final {
        left_upstream_operator_->Open();
        right_upstream_operator_->Open();
        auto tuple_res = left_upstream_operator_->Next();
        while (tuple_res.second == tuple_flag::valid) {
            ht.emplace(tuple_res.first.key, tuple_res.first.value);
            tuple_res = left_upstream_operator_->Next();
        }
    }

    Optional<Tuple> INLINE next() {
        auto tuple_res = right_upstream_operator_->Next();
        while (tuple_res.second == tuple_flag::valid) {
            if (ht.count(tuple_res.first.key)) {
                return std::make_pair(tuple_res.first, tuple_flag::valid);
            }
            tuple_res = right_upstream_operator_->Next();
        }

        return std::make_pair(data::Tuple{0, 0}, tuple_flag::invalid);
    }

    void open() {
        upstream1->open();
        upstream2->open();
    }

    void close() {
        upstream1->close();
        upstream2->close();
    }
private:
    std::unordered_map<uint32_t, uint32_t> ht;
};


template<class Upstream1, class Upstream2, class Tuple>
JoinOperator<Upstream1, Upstream2, Tuple> makeJoinOperator(Upstream1 *upstream1, Upstream2 *upstream2) {
    return JoinOperator<Upstream1, Upstream2, Tuple>(upstream1, upstream2);
};


#endif //CPP_JOINOPERATOR_H
