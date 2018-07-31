#ifndef OPTIMIZE_OPTIMIZER_HPP
#define OPTIMIZE_OPTIMIZER_HPP

#include <string>

class DAG;

namespace optimize {

class Optimizer {
public:
    explicit Optimizer(std::string config);
    void Run(DAG* dag);

private:
    const std::string config_;
};

}  // namespace optimize

#endif  // OPTIMIZE_OPTIMIZER_HPP
