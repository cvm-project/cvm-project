#ifndef DAG_COLLECTION_TUPLE_HPP
#define DAG_COLLECTION_TUPLE_HPP

#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "dag/type/tuple.hpp"
#include "field.hpp"

namespace dag {
namespace collection {

struct Tuple {
    explicit Tuple(const nlohmann::json &json) { this->from_json(json); }

    Tuple(const Tuple &other) {
        type = other.type;
        for (const auto &e : other.fields) {
            fields.push_back(std::make_unique<Field>(*e));
        }
    }

    Tuple() = delete;
    ~Tuple() = default;
    Tuple(Tuple &&field) = default;
    Tuple &operator=(const Tuple &rhs) = delete;
    Tuple &operator=(Tuple &&rhs) = default;

    void from_json(const nlohmann::json &json);

    const type::Tuple *type{};
    std::vector<std::unique_ptr<Field>> fields{};
};

}  // namespace collection
}  // namespace dag
#endif  // DAG_COLLECTION_TUPLE_HPP
