#include "utils.hpp"

#include <boost/mpl/list.hpp>

#include "dag/operators/column_scan.hpp"
#include "dag/operators/range.hpp"
#include "dag/operators/row_scan.hpp"
#include "dag/operators/split_column_data.hpp"
#include "dag/operators/split_range.hpp"
#include "dag/operators/split_row_data.hpp"
#include "utils/visitor.hpp"

namespace optimize {

auto MakeSplitOperator(const DAGOperator *const op) -> DAGOperator * {
    class MakeSplitOperatorVisitor
        : public Visitor<MakeSplitOperatorVisitor, const DAGOperator,
                         boost::mpl::list<       //
                                 DAGColumnScan,  //
                                 DAGRowScan,     //
                                 DAGRange        //
                                 >::type,
                         DAGOperator *> {
    public:
        auto operator()(const DAGColumnScan * /*op*/) -> DAGOperator * {
            return new DAGSplitColumnData();
        }
        auto operator()(const DAGRowScan * /*op*/) -> DAGOperator * {
            return new DAGSplitRowData();
        }
        auto operator()(const DAGRange * /*op*/) -> DAGOperator * {
            return new DAGSplitRange();
        }
    };
    return MakeSplitOperatorVisitor().Visit(op);
}

}  // namespace optimize
