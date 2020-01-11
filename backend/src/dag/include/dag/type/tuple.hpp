#ifndef DAG_TYPE_TUPLE_HPP
#define DAG_TYPE_TUPLE_HPP

#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "field_type.hpp"

namespace dag {
namespace type {

struct Tuple : public Type {
protected:
    Tuple() = default;
    friend struct nlohmann::adl_serializer<std::unique_ptr<Type>>;

public:
    static auto MakeTuple(const std::vector<const FieldType *> &field_types)
            -> const Tuple *;

    [[nodiscard]] auto ComputeHeadTuple(size_t head_size = 1) const
            -> const Tuple *;
    [[nodiscard]] auto ComputeTailTuple(size_t head_size = 1) const
            -> const Tuple *;

    [[nodiscard]] auto to_string() const -> std::string override;
    void from_json(const nlohmann::json &json) override;
    void to_json(nlohmann::json *json) const override;

    std::vector<const FieldType *> field_types;
};

}  // namespace type
}  // namespace dag

#endif  // DAG_TYPE_TUPLE_HPP
