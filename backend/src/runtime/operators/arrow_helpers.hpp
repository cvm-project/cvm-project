#ifndef RUNTIME_OPERATORS_ARROW_HELPERS_HPP
#define RUNTIME_OPERATORS_ARROW_HELPERS_HPP

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <arrow/status.h>
#include <arrow/type.h>
#include <arrow/util/macros.h>

#define ARROW_THROW_IF_(condition, status, _)                    \
    do {                                                         \
        if (ARROW_PREDICT_FALSE(condition)) {                    \
            ::arrow::Status __s = (status);                      \
            throw std::runtime_error(__s.CodeAsString() + ": " + \
                                     __s.message());             \
        }                                                        \
    } while (0)

#define ARROW_TRHOW_NOT_OK(status)                                           \
    do {                                                                     \
        ::arrow::Status _st = (status);                                      \
        ARROW_THROW_IF_(!_st.ok(), std::move(_st), ARROW_STRINGIFY(status)); \
    } while (false)

namespace runtime {
namespace operators {

std::shared_ptr<arrow::Schema> MakeArrowSchema(
        std::vector<std::string> column_types,
        std::vector<std::string> column_names);

static const std::map<std::string, std::shared_ptr<arrow::DataType>>&
arrow_types();

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_OPERATORS_ARROW_HELPERS_HPP
