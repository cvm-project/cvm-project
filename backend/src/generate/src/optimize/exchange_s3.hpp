#ifndef OPTIMIZE_EXCHANGE_S3_HPP
#define OPTIMIZE_EXCHANGE_S3_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ExchangeS3 : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "exchange_s3";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_EXCHANGE_S3_HPP
