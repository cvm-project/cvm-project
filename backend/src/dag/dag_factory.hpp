//
// Created by sabir on 04.07.17.
//

#ifndef DAG_DAGCREATION_HPP
#define DAG_DAGCREATION_HPP

#include <iostream>
#include <string>

#include "DAG.h"
#include "DAGOperator.h"

using make_dag_function = DAGOperator *(*)();  // function pointer type
using DagOperatorsMap = std::unordered_map<std::string, make_dag_function>;

class DagFactory {
public:
    static DagFactory &instance() {
        static DagFactory dc;
        return dc;
    }
    static DAGOperator *MakeOperator(const std::string &opName);

    DAG *ParseDag(std::istream *istream);
    DAG *ParseDag(const std::string &dagstr);

private:
    DagFactory() { load_operators(); };

    DagOperatorsMap op_map_;
    void load_operators();

    struct LoadOperatorFunctor {
        template <class OperatorType>
        void operator()(OperatorType * /*unused*/);

        DagFactory *const factory;
    };
};

#endif  // DAG_DAGCREATION_HPP
