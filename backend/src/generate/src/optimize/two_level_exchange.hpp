#ifndef OPTIMIZE_TWO_LEVEL_EXCHANGE_HPP
#define OPTIMIZE_TWO_LEVEL_EXCHANGE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class TwoLevelExchange : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "two_level_exchange";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_TWO_LEVEL_EXCHANGE_HPP
