#ifndef OPTIMIZE_PARALLELIZE_CONCURRENT_TCP_HPP
#define OPTIMIZE_PARALLELIZE_CONCURRENT_TCP_HPP

#include "dag_transformation.hpp"

namespace optimize {

class ExchangeTcp : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "exchange_tcp";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_PARALLELIZE_CONCURRENT_TCP_HPP
