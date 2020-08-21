#ifndef DAG_OPERATORS_ALL_OPERATOR_DECLARATIONS_HPP
#define DAG_OPERATORS_ALL_OPERATOR_DECLARATIONS_HPP

#include <type_traits>

#include <boost/mpl/list.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform_view.hpp>

class DAGAntiJoin;
class DAGAntiJoinPredicated;
class DAGAssertCorrectOpenNextClose;
class DAGBroadcast;
class DAGCartesian;
class DAGColumnScan;
class DAGCompiledPipeline;
class DAGConcurrentExecute;
class DAGConcurrentExecuteLambda;
class DAGConcurrentExecuteProcess;
class DAGConstantTuple;
class DAGEnsureSingleTuple;
class DAGExchange;
class DAGExchangeS3;
class DAGExchangeTcp;
class DAGExpandPattern;
class DAGFilter;
class DAGGroupBy;
class DAGJoin;
class DAGMap;
class DAGMapCpp;
class DAGMaterializeColumnChunks;
class DAGMaterializeParquet;
class DAGMaterializeRowVector;
class DAGNestedMap;
class DAGParallelMap;
class DAGParallelMapOmp;
class DAGParameterLookup;
class DAGParquetScan;
class DAGPartition;
class DAGPartitionedExchange;
class DAGPipeline;
class DAGProjection;
class DAGRange;
class DAGReduce;
class DAGReduceByIndex;
class DAGReduceByKey;
class DAGReduceByKeyGrouped;
class DAGRowScan;
class DAGSemiJoin;
class DAGSplitColumnData;
class DAGSplitRange;
class DAGSplitRowData;
class DAGSort;
class DAGTopK;
class DAGZip;

namespace dag {

using AllOperatorTypes = boost::mpl::list<  //
        DAGAntiJoin,                        //
        DAGAntiJoinPredicated,              //
        DAGAssertCorrectOpenNextClose,      //
        DAGBroadcast,                       //
        DAGCartesian,                       //
        DAGColumnScan,                      //
        DAGCompiledPipeline,                //
        DAGConcurrentExecute,               //
        DAGConcurrentExecuteLambda,         //
        DAGConcurrentExecuteProcess,        //
        DAGConstantTuple,                   //
        DAGEnsureSingleTuple,               //
        DAGExchange,                        //
        DAGExchangeS3,                      //
        DAGExchangeTcp,                     //
        DAGExpandPattern,                   //
        DAGFilter,                          //
        DAGGroupBy,                         //
        DAGJoin,                            //
        DAGMap,                             //
        DAGMapCpp,                          //
        DAGMaterializeColumnChunks,         //
        DAGMaterializeParquet,              //
        DAGMaterializeRowVector,            //
        DAGNestedMap,                       //
        DAGParallelMap,                     //
        DAGParallelMapOmp,                  //
        DAGParameterLookup,                 //
        DAGParquetScan,                     //
        DAGPartition,                       //
        DAGPartitionedExchange,             //
        DAGPipeline,                        //
        DAGProjection,                      //
        DAGRange,                           //
        DAGReduce,                          //
        DAGReduceByIndex,                   //
        DAGReduceByKey,                     //
        DAGReduceByKeyGrouped,              //
        DAGRowScan,                         //
        DAGSemiJoin,                        //
        DAGSplitColumnData,                 //
        DAGSplitRange,                      //
        DAGSplitRowData,                    //
        DAGSort,                            //
        DAGTopK,                            //
        DAGZip                              //
        >::type;

using AllOperatorPointerTypes = typename boost::mpl::transform_view<
        AllOperatorTypes, std::add_pointer<boost::mpl::placeholders::_>>::type;

}  // namespace dag

#endif  // DAG_OPERATORS_ALL_OPERATOR_DECLARATIONS_HPP
