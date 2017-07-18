//
// Created by sabir on 18.07.17.
//

#include "BuildDAG.hpp"
#include <tuple>
#include "vector"

#include "operators/CollectionSourceOperator.h"
#include "operators/MapOperator.h"
#include "functions/MapFunction0.hpp"

Operator *build_dag() {

    typedef std::tuple<long, double, double> t0;

    std::vector<t0> v1;
    v1.push_back(t0(1, 2.3, 4.2));
    v1.push_back(t0(3, 2.5, 4.1));

    auto op1 = makeCollectionSourceOperator<t0>();
    op1.values = v1;

    typedef std::tuple<double, double> t1;
    auto op2 = makeMapOperator<t1>(&op1, MapFunction0());

    op2.open();
    while (auto res = op2.next()) {
        std::cout << std::get<0>(res.value) << ", " << std::get<1>(res.value) << std::endl;
    }

    auto m = malloc(sizeof(op2));
    memmove(m, (void*)&op2, sizeof(op2));
    return (Operator *) m;
}