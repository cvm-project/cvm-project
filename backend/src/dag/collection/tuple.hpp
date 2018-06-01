//
// Created by sabir on 22.05.18.
//

#ifndef CPP_TUPLE_HPP
#define CPP_TUPLE_HPP

#include <memory>
#include <vector>

#include <json.hpp>

#include "dag/type/tuple.hpp"
#include "field.hpp"

namespace dag {
namespace collection {

struct Tuple {
    explicit Tuple(const nlohmann::json &json) { this->from_json(json); }

    void from_json(const nlohmann::json &json);

    const type::Tuple *type{};
    std::vector<std::shared_ptr<Field>> fields;
};

}  // namespace collection
}  // namespace dag
#endif  // CPP_TUPLE_HPP
