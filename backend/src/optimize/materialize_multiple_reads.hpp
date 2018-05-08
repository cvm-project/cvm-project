#ifndef OPTIMIZE_MATERIALIZE_MULTIPLE_READS_H
#define OPTIMIZE_MATERIALIZE_MULTIPLE_READS_H

#include "dag/DAG.h"

// cppcheck-suppress noConstructor
class MaterializeMultipleReads {
public:
    explicit MaterializeMultipleReads(DAG* const dag) : dag_(dag) {}

    void optimize();

private:
    DAG* const dag_;
};

#endif  // OPTIMIZE_MATERIALIZE_MULTIPLE_READS_H
