#include "materialize_parquet.hpp"

void DAGMaterializeParquet::from_json(const nlohmann::json &json) {
    column_names = json.at("column_names").get<decltype(column_names)>();
}

void DAGMaterializeParquet::to_json(nlohmann::json *json) const {
    json->emplace("column_names", column_names);
}
