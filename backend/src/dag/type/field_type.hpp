#ifndef JITQ_DAG_TYPE_FIELD_TYPE_HPP
#define JITQ_DAG_TYPE_FIELD_TYPE_HPP

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

#endif  // JITQ_DAG_TYPE_FIELD_TYPE_HPP
