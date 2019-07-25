#ifndef DAG_ALL_OPERATOR_DECLARATIONS_HPP
#define DAG_ALL_OPERATOR_DECLARATIONS_HPP

#include <type_traits>

#include <boost/mpl/list.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform_view.hpp>

class DAGAssertCorrectOpenNextClose;
class DAGCartesian;
class DAGColumnScan;
class DAGConstantTuple;
class DAGEnsureSingleTuple;
class DAGExpandPattern;
class DAGFilter;
class DAGGroupBy;
class DAGJoin;
class DAGMap;
class DAGMaterializeColumnChunks;
class DAGMaterializeParquet;
class DAGMaterializeRowVector;
class DAGParallelMap;
class DAGParameterLookup;
class DAGParquetScan;
class DAGPartition;
class DAGPipeline;
class DAGProjection;
class DAGRange;
class DAGReduce;
class DAGReduceByKey;
class DAGReduceByKeyGrouped;
class DAGRowScan;
class DAGSplitColumnData;
class DAGSplitRange;
class DAGSplitRowData;

namespace dag {

using AllOperatorTypes = boost::mpl::list<  //
        DAGAssertCorrectOpenNextClose,      //
        DAGCartesian,                       //
        DAGColumnScan,                      //
        DAGConstantTuple,                   //
        DAGEnsureSingleTuple,               //
        DAGExpandPattern,                   //
        DAGFilter,                          //
        DAGGroupBy,                         //
        DAGJoin,                            //
        DAGMap,                             //
        DAGMaterializeColumnChunks,         //
        DAGMaterializeParquet,              //
        DAGMaterializeRowVector,            //
        DAGParallelMap,                     //
        DAGParameterLookup,                 //
        DAGParquetScan,                     //
        DAGPartition,                       //
        DAGPipeline,                        //
        DAGProjection,                      //
        DAGRange,                           //
        DAGReduce,                          //
        DAGReduceByKey,                     //
        DAGReduceByKeyGrouped,              //
        DAGRowScan,                         //
        DAGSplitColumnData,                 //
        DAGSplitRange,                      //
        DAGSplitRowData                     //
        >::type;

using AllOperatorPointerTypes = typename boost::mpl::transform_view<
        AllOperatorTypes, std::add_pointer<boost::mpl::placeholders::_>>::type;

}  // namespace dag

#endif  // DAG_ALL_OPERATOR_DECLARATIONS_HPP
