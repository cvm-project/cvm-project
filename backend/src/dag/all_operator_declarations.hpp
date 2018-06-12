#ifndef DAG_ALL_OPERATOR_DECLARATIONS_HPP
#define DAG_ALL_OPERATOR_DECLARATIONS_HPP

#include <type_traits>

#include <boost/mpl/list.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform_view.hpp>

class DAGAssertCorrectOpenNextClose;
class DAGCartesian;
class DAGCollection;
class DAGConstantTuple;
class DAGEnsureSingleTuple;
class DAGFilter;
class DAGJoin;
class DAGMap;
class DAGMaterializeRowVector;
class DAGParallelMap;
class DAGParameterLookup;
class DAGPipeline;
class DAGRange;
class DAGReduce;
class DAGReduceByKey;
class DAGReduceByKeyGrouped;
class DAGSplitCollection;
class DAGSplitRange;

namespace dag {

using AllOperatorTypes = boost::mpl::list<  //
        DAGAssertCorrectOpenNextClose,      //
        DAGCartesian,                       //
        DAGCollection,                      //
        DAGConstantTuple,                   //
        DAGEnsureSingleTuple,               //
        DAGFilter,                          //
        DAGJoin,                            //
        DAGMap,                             //
        DAGMaterializeRowVector,            //
        DAGParallelMap,                     //
        DAGParameterLookup,                 //
        DAGPipeline,                        //
        DAGRange,                           //
        DAGReduce,                          //
        DAGReduceByKey,                     //
        DAGReduceByKeyGrouped,              //
        DAGSplitCollection,                 //
        DAGSplitRange                       //
        >::type;

using AllOperatorPointerTypes = typename boost::mpl::transform_view<
        AllOperatorTypes, std::add_pointer<boost::mpl::placeholders::_>>::type;

}  // namespace dag

#endif  // DAG_ALL_OPERATOR_DECLARATIONS_HPP
