#ifndef CPP_ATOMIC_HPP
#define CPP_ATOMIC_HPP

#include "field.hpp"

#include "dag/type/atomic.hpp"

namespace dag {
namespace collection {

class Atomic : public FieldBase<Atomic, type::Atomic> {
public:
    explicit Atomic(const type::FieldType* field_type, const size_t& position)
        : FieldBase(field_type, position){};
};

}  // namespace collection
}  // namespace dag

#endif  // CPP_ATOMIC_HPP
