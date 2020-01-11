#ifndef DAG_UTILS_TYPE_TRAITS_HPP
#define DAG_UTILS_TYPE_TRAITS_HPP

#include <type_traits>

#include <boost/mpl/contains.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/transform_view.hpp>

#include "utils/visitor.hpp"

class DAG;
class DAGOperator;

namespace dag {
namespace utils {

namespace impl {

template <typename... OperatorTypes>
class IsInstanceOfVisitor;

}  // namespace impl

template <typename... OperatorTypes>
auto IsInstanceOf(const DAGOperator *const op) -> bool {
    return impl::IsInstanceOfVisitor<OperatorTypes...>().Visit(op);
}

namespace impl {

template <typename... OperatorTypes>
class IsInstanceOfVisitor
    : public Visitor<IsInstanceOfVisitor<OperatorTypes...>, const DAGOperator,
                     typename boost::mpl::list<OperatorTypes...>::type, bool> {
public:
    using OperatorTypeList = typename boost::mpl::transform_view<
            boost::mpl::list<OperatorTypes...>,
            std::remove_cv<boost::mpl::placeholders::_>>::type;
    template <class OperatorType,
              typename = std::enable_if_t<boost::mpl::contains<
                      OperatorTypeList, std::remove_cv_t<OperatorType>>::value>>
    auto operator()(OperatorType *const /*op*/) -> bool {
        return true;
    }
    auto operator()(DAGOperator *const /*op*/) -> bool { return false; }
};

}  // namespace impl

auto IsPipelineDriver(DAGOperator *op) -> bool;
auto IsPipelinePredecessor(DAGOperator *op) -> bool;
auto IsSingleTupleProducer(DAGOperator *op) -> bool;
auto IsSourceOperator(DAGOperator *op) -> bool;

}  // namespace utils
}  // namespace dag

#endif  // DAG_UTILS_TYPE_TRAITS_HPP
