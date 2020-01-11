#ifndef OPTIMIZE_CODE_GEN_HPP
#define OPTIMIZE_CODE_GEN_HPP

#include <string>

#include "dag_transformation.hpp"

namespace optimize {

class CodeGen : public DagTransformation {
public:
    void Run(DAG *dag, const std::string &config) const override;
    [[nodiscard]] auto name() const -> std::string override {
        return "code_gen";
    }
};

}  // namespace optimize

#endif  // OPTIMIZE_CODE_GEN_HPP
