#ifndef DAG_TYPE_FIELD_TYPE_HPP
#define DAG_TYPE_FIELD_TYPE_HPP

#include "type.hpp"
#include "utils/utils.h"

namespace dag {
namespace type {

struct FieldType : public Type {
protected:
    FieldType() = default;
};

}  // namespace type
}  // namespace dag

#endif  // DAG_TYPE_FIELD_TYPE_HPP
