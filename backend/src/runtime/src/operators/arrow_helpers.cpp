#include "arrow_helpers.hpp"

#include <cassert>

#include <memory>
#include <string>
#include <vector>

#include <arrow/type.h>

namespace runtime {
namespace operators {

std::shared_ptr<arrow::Schema> MakeArrowSchema(
        std::vector<std::string> column_types,
        std::vector<std::string> column_names) {
    assert(column_types.size() == column_names.size());
    std::vector<std::shared_ptr<arrow::Field>> fields;
    for (size_t i = 0; i < column_types.size(); i++) {
        fields.push_back(arrow::field(column_names.at(i),
                                      arrow_types().at(column_types.at(i)),
                                      false));
    }
    return std::make_shared<arrow::Schema>(std::move(fields));
}

const std::map<std::string, std::shared_ptr<arrow::DataType>>& arrow_types() {
    static const std::map<std::string, std::shared_ptr<arrow::DataType>> map = {
            {"int", arrow::int32()},      //
            {"long", arrow::int64()},     //
            {"float", arrow::float32()},  //
            {"double", arrow::float64()}};
    return map;
}

}  // namespace operators
}  // namespace runtime
