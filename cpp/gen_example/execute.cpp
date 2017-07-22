//
// Created by sabir on 18.07.17.
//

#include <tuple>
#include <vector>

#include "operators/Operator.h"
#include "operators/CollectionSourceOperator.h"
#include "operators/MapOperator.h"
#include "functions/MapFunction0.hpp"

using namespace std;

void execute() {

    typedef tuple<long, long, long, long> t0;

    std::vector<t0> v1;
    v1.push_back(t0(1, 2, 3, 4));
    v1.push_back(t0(5, 6, 7, 8));

    auto op1 = makeCollectionSourceOperator<t0>();
    op1.values = v1;

    typedef tuple<long, long> t1;
    auto op2 = makeMapOperator<t1>(&op1, MapFunction0());

    op2.open();
    while (auto res = op2.next()) {
        cout << get<0>(res.value) << ", " << get<1>(res.value) << std::endl;
    }
}