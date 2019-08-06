#ifndef DAG_COLLECTION_ARRAY_HPP
#define DAG_COLLECTION_ARRAY_HPP

#include <memory>

#include <nlohmann/json.hpp>

#include "dag/type/array.hpp"
#include "field.hpp"

namespace dag {
namespace collection {

struct Tuple;
class Array : public FieldBase<Array, type::Array> {
public:
    explicit Array(const type::FieldType *field_type_, const size_t &position,
                   const nlohmann::json &json);

    Tuple *tuple() { return tuple_.get(); }

private:
    std::unique_ptr<Tuple> tuple_;
};

}  // namespace collection
}  // namespace dag

#endif  // DAG_COLLECTION_ARRAY_HPP
