#ifndef OPTIMIZE_ATTRIBUTEID_TRACKING_HPP
#define OPTIMIZE_ATTRIBUTEID_TRACKING_HPP

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class AttributeIdTracking {
public:
    explicit AttributeIdTracking(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_ATTRIBUTEID_TRACKING_HPP
