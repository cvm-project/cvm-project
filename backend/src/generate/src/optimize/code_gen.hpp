#ifndef OPTIMIZE_CODE_GEN_HPP
#define OPTIMIZE_CODE_GEN_HPP

#include <string>

#include "dag_transformation.hpp"

namespace optimize {

class CodeGen : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    std::string name() const override { return "code_gen"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_CODE_GEN_HPP
