#ifndef DAG_OPERATORS_ALL_OPERATOR_DEFINITIONS_HPP
#define DAG_OPERATORS_ALL_OPERATOR_DEFINITIONS_HPP

#include "antijoin.hpp"
#include "antijoin_predicated.hpp"
#include "assert_correct_open_next_close.hpp"
#include "cartesian.hpp"
#include "column_scan.hpp"
#include "compiled_pipeline.hpp"
#include "constant_tuple.hpp"
#include "ensure_single_tuple.hpp"
#include "expand_pattern.hpp"
#include "filter.hpp"
#include "group_by.hpp"
#include "join.hpp"
#include "map.hpp"
#include "materialize_column_chunks.hpp"
#include "materialize_parquet.hpp"
#include "materialize_row_vector.hpp"
#include "parallel_map.hpp"
#include "parallel_map_omp.hpp"
#include "parameter_lookup.hpp"
#include "parquet_scan.hpp"
#include "partition.hpp"
#include "pipeline.hpp"
#include "projection.hpp"
#include "range.hpp"
#include "reduce.hpp"
#include "reduce_by_index.hpp"
#include "reduce_by_key.hpp"
#include "reduce_by_key_grouped.hpp"
#include "row_scan.hpp"
#include "semijoin.hpp"
#include "sort.hpp"
#include "split_column_data.hpp"
#include "split_range.hpp"
#include "split_row_data.hpp"
#include "topk.hpp"
#include "zip.hpp"

#endif
