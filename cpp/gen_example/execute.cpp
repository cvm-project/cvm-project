/**
 * Auto-generated execution plan
 */

#include <tuple>
#include <vector>

#include "operators/Operator.h"
#include "operators/CollectionSourceOperator.h"
#include "operators/MapOperator.h"
#include "functions/MapFunction0.hpp"
#include "utils/tuple_to_string.cpp"

using namespace std;

void execute() {

    typedef tuple<long, long, long, long> t0;

    std::vector<t0> v1;
    v1.push_back(t0(1, 2, 3, 4));
    v1.push_back(t0(5, 6, 7, 8));
    /**COLLECTION SOURCE **/
    auto op1 = makeCollectionSourceOperator<t0>();
    op1.values = v1;

    typedef tuple<long, long> t1;
    auto op2 = makeMapOperator<t1>(&op1, MapFunction0());

    op2.open();
    while (auto res = op2.next()) {
        cout << to_string(res.value) << endl;
    }
}