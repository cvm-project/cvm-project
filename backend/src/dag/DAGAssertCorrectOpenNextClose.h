#ifndef DAG_DAGASSERTCORRECTOPENNEXTCLOSE_H
#define DAG_DAGASSERTCORRECTOPENNEXTCLOSE_H

#include <json.hpp>

#include "DAGOperator.h"

class DAGAssertCorrectOpenNextClose
    : public DAGOperatorBase<DAGAssertCorrectOpenNextClose> {
public:
    constexpr static const char *kName = "assert_correct_open_next_close";
    constexpr static size_t kNumInPorts = 1;  // XXX: make this dynamic
    constexpr static size_t kNumOutPorts = 1;
};

#endif  // DAG_DAGASSERTCORRECTOPENNEXTCLOSE_H
