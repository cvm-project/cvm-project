#include <type_traits>

#include <boost/mpl/list.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform_view.hpp>

class DAGCartesian;
class DAGCollection;
class DAGConstantTuple;
class DAGEnsureSingleTuple;
class DAGFilter;
class DAGJoin;
class DAGMap;
class DAGMaterializeRowVector;
class DAGParameterLookup;
class DAGRange;
class DAGReduce;
class DAGReduceByKey;
class DAGReduceByKeyGrouped;

namespace dag {

using AllOperatorTypes = boost::mpl::list<  //
        DAGCartesian,                       //
        DAGCollection,                      //
        DAGConstantTuple,                   //
        DAGEnsureSingleTuple,               //
        DAGFilter,                          //
        DAGJoin,                            //
        DAGMap,                             //
        DAGMaterializeRowVector,            //
        DAGParameterLookup,                 //
        DAGRange,                           //
        DAGReduce,                          //
        DAGReduceByKey,                     //
        DAGReduceByKeyGrouped               //
        >::type;

using AllOperatorPointerTypes = typename boost::mpl::transform_view<
        AllOperatorTypes, std::add_pointer<boost::mpl::placeholders::_>>::type;

}  // namespace dag
