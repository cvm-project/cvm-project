//
// Created by sabir on 22.05.18.
//

#ifndef CPP_ARRAY_HPP
#define CPP_ARRAY_HPP

#include <memory>

#include <json.hpp>

#include "dag/type/array.hpp"
#include "field.hpp"

namespace dag {
namespace collection {

struct Tuple;
class Array : public FieldBase<Array, type::Array> {
public:
    explicit Array(const type::FieldType *field_type_, const size_t &position,
                   const nlohmann::json &json);

    std::shared_ptr<Tuple> tuple() { return tuple_; }

private:
    std::shared_ptr<Tuple> tuple_;
};

}  // namespace collection
}  // namespace dag

#endif  // CPP_ARRAY_HPP
