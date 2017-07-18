//
// Created by sabir on 04.07.17.
//

#ifndef CPP_DAGRANGE_H
#define CPP_DAGRANGE_H

//#include "operators/RangeSourceOperator.h"
#include <operators/RangeSourceOperator.h>
#include "DAGOperator.h"

class DAGRange : public DAGOperator {
public:
    static DAGOperator *make_dag_operator() {
        return new DAGRange;
    };

    template<class Tuple>
    RangeSourceOperator<Tuple> make_operator() {
        return RangeSourceOperator<Tuple>(start, end, step);
    }

    void initWithJson(nlohmann::basic_json<>) {

    };

private:
    int start;
    int end;
    int step = 1;
};


#endif //CPP_DAGRANGE_H
