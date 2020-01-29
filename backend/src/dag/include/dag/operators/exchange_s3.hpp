#ifndef DAG_OPERATORS_EXCHANGE_S3_HPP
#define DAG_OPERATORS_EXCHANGE_S3_HPP

#include "operator.hpp"

class DAGExchangeS3 : public DAGOperator {
    // cppcheck-suppress noExplicitConstructor  // false positive
    JITQ_DAGOPERATOR(DAGExchangeS3, "exchange_s3");

public:
    [[nodiscard]] auto num_in_ports() const -> size_t override { return 3; }
    [[nodiscard]] auto num_out_ports() const -> size_t override { return 1; }

    size_t num_levels = 1;
    size_t level_num = 0;
};

#endif  // DAG_OPERATORS_EXCHANGE_S3_HPP
