#ifndef OPTIMIZE_ADD_ALWAYS_INLINE_HPP
#define OPTIMIZE_ADD_ALWAYS_INLINE_HPP

#include "dag_transformation.hpp"

namespace optimize {

class AddAlwaysInline : public DagTransformation {
public:
    void Run(DAG *dag) const override;
    std::string name() const override { return "add_always_inline"; }
};

}  // namespace optimize

#endif  // OPTIMIZE_ADD_ALWAYS_INLINE_HPP
