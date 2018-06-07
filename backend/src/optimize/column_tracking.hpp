#ifndef OPTIMIZE_COLUMN_TRACKING_HPP
#define OPTIMIZE_COLUMN_TRACKING_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class ColumnTracking {
public:
    explicit ColumnTracking(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_COLUMN_TRACKING_HPP
