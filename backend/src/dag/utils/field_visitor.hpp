//
// Created by sabir on 16.05.18.
//

#ifndef CODE_GEN_COMMON_FIELD_TYPE_VISITOR_HPP
#define CODE_GEN_COMMON_FIELD_TYPE_VISITOR_HPP

#include "dag/collection/field.hpp"

namespace dag {

namespace collection {
class Atomic;
class Array;
}  // namespace collection

namespace utils {

class FieldVisitor {
public:
    template <class FieldClass, class FieldTypeClass>
    friend class dag::collection::FieldBase;

    virtual ~FieldVisitor() = default;

protected:
    virtual void Visit(dag::collection::Atomic *field) = 0;
    virtual void Visit(dag::collection::Array *field) = 0;
};

}  // namespace utils

namespace collection {

template <class FieldClass, class FieldTypeClass>
void FieldBase<FieldClass, FieldTypeClass>::Accept(
        utils::FieldVisitor *visitor) {
    visitor->Visit(reinterpret_cast<FieldClass *>(this));
}

}  // namespace collection
}  // namespace dag

#endif  // CODE_GEN_COMMON_FIELD_TYPE_VISITOR_HPP
