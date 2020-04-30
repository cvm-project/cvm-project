#ifndef DAG_OPERATORS_ALL_OPERATOR_DECLARATIONS_HPP
#define DAG_OPERATORS_ALL_OPERATOR_DECLARATIONS_HPP

#include <type_traits>

#include <boost/mpl/list.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/transform_view.hpp>

class DAGAntiJoin;
class DAGAssertCorrectOpenNextClose;
class DAGCartesian;
class DAGColumnScan;
class DAGCompiledPipeline;
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
class DAGParallelMapOmp;
class DAGParameterLookup;
class DAGParquetScan;
class DAGPartition;
class DAGPipeline;
class DAGProjection;
class DAGRange;
class DAGReduce;
class DAGReduceByIndex;
class DAGReduceByKey;
class DAGReduceByKeyGrouped;
class DAGRowScan;
class DAGSplitColumnData;
class DAGSplitRange;
class DAGSplitRowData;
class DAGZip;

namespace dag {

using AllOperatorTypes = boost::mpl::list<  //
        DAGAntiJoin,                        //
        DAGAssertCorrectOpenNextClose,      //
        DAGCartesian,                       //
        DAGColumnScan,                      //
        DAGCompiledPipeline,                //
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
        DAGParallelMapOmp,                  //
        DAGParameterLookup,                 //
        DAGParquetScan,                     //
        DAGPartition,                       //
        DAGPipeline,                        //
        DAGProjection,                      //
        DAGRange,                           //
        DAGReduce,                          //
        DAGReduceByIndex,                   //
        DAGReduceByKey,                     //
        DAGReduceByKeyGrouped,              //
        DAGRowScan,                         //
        DAGSplitColumnData,                 //
        DAGSplitRange,                      //
        DAGSplitRowData,                    //
        DAGZip                              //
        >::type;

using AllOperatorPointerTypes = typename boost::mpl::transform_view<
        AllOperatorTypes, std::add_pointer<boost::mpl::placeholders::_>>::type;

}  // namespace dag

#endif  // DAG_OPERATORS_ALL_OPERATOR_DECLARATIONS_HPP
