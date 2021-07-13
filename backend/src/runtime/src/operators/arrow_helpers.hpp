#ifndef OPERATORS_ARROW_HELPERS_HPP
#define OPERATORS_ARROW_HELPERS_HPP

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/type.h>
#include <boost/config.hpp>

namespace runtime {
namespace operators {

void ThrowIfNotOK(const arrow::Status &status);

template <class T>
void ThrowIfNotOK(const arrow::Result<T> &result) {
    if (BOOST_UNLIKELY(!result.ok())) {
        throw std::runtime_error(result.status().CodeAsString() + ": " +
                                 result.status().message());
    }
}

auto MakeArrowSchema(std::vector<std::string> column_types,
                     std::vector<std::string> column_names)
        -> std::shared_ptr<arrow::Schema>;

static auto arrow_types()
        -> const std::map<std::string, std::shared_ptr<arrow::DataType>> &;

}  // namespace operators
}  // namespace runtime

#endif  // OPERATORS_ARROW_HELPERS_HPP
