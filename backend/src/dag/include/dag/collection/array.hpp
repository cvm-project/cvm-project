#ifndef DAG_COLLECTION_ARRAY_HPP
#define DAG_COLLECTION_ARRAY_HPP

#include <memory>

#include "dag/type/array.hpp"
#include "field.hpp"

namespace dag {
namespace collection {

struct Tuple;
class Array : public FieldBase<Array, type::Array> {
public:
    Array(const type::Array *array_type, const size_t &position);

    Tuple *tuple() { return tuple_.get(); }

private:
    std::unique_ptr<Tuple> tuple_;
};

}  // namespace collection
}  // namespace dag

#endif  // DAG_COLLECTION_ARRAY_HPP
