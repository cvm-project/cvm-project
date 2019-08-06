#include "dag/operators/parquet_scan.hpp"

void DAGParquetScan::from_json(const nlohmann::json &json) {
    for (const auto &c : json.at("columns")) {
        RangeFilters ranges;
        for (const auto &r : c.at("ranges")) {
            ranges.emplace_back(r.at("lo"), r.at("hi"));
        }
        column_range_filters.emplace_back(ranges);
        column_indexes.emplace_back(c.at("idx"));
    }
    filesystem = json.at("filesystem");
}

void DAGParquetScan::to_json(nlohmann::json *json) const {
    assert(column_indexes.size() == column_range_filters.size());
    auto columns = nlohmann::json::array();
    for (size_t i = 0; i < column_indexes.size(); i++) {
        nlohmann::json col = {
                // cppcheck-suppress constStatement
                {"idx", column_indexes[i]},  // cppcheck-suppress constStatement
                {"ranges", nlohmann::json::array()}};
        for (const auto &r : column_range_filters[i]) {
            col["ranges"].push_back({{"lo", r.first}, {"hi", r.second}});
        }
        columns.push_back(col);
    }
    json->emplace("columns", columns);
    json->emplace("filesystem", filesystem);
}
