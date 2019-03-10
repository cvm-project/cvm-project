#include "add_always_inline.hpp"

#include "dag/DAGOperator.h"
#include "dag/utils/apply_visitor.hpp"
#include "llvm_helpers/function.hpp"

void AddInline(DAGOperator *const op, const DAG *const /*dag*/) {
    if (!op->llvm_ir.empty()) {
        llvm_helpers::Function function(op->llvm_ir);
        op->llvm_ir = function.AddInlineAttribute();
    }
}

namespace optimize {
void AddAlwaysInline::Run(DAG *const dag,
                          const std::string & /*config*/) const {
    dag::utils::ApplyInTopologicalOrderRecursively(dag, AddInline);
}
}  // namespace optimize