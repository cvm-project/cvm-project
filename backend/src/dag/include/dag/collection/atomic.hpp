#ifndef DAG_COLLECTION_ATOMIC_HPP
#define DAG_COLLECTION_ATOMIC_HPP

#include "dag/type/atomic.hpp"
#include "field.hpp"

namespace dag {
namespace collection {

class Atomic : public FieldBase<Atomic, type::Atomic> {
public:
    explicit Atomic(const type::FieldType* field_type, const size_t& position)
        : FieldBase(field_type, position){};
};

}  // namespace collection
}  // namespace dag

#endif  // DAG_COLLECTION_ATOMIC_HPP
