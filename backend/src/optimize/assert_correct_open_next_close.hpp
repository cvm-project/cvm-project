#ifndef OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_H
#define OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_H

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class AssertCorrectOpenNextClose {
public:
    explicit AssertCorrectOpenNextClose(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_ASSERT_CORRECT_OPEN_NEXT_CLOSE_H
