//
// Created by sabir on 11.05.18.
//

#ifndef DAG_TYPE_ARRAY_HPP
#define DAG_TYPE_ARRAY_HPP

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "field_type.hpp"

namespace dag {
namespace type {

struct Tuple;

enum ArrayLayout { kC, kF };
auto to_string(const ArrayLayout &layout) -> std::string;

struct Array : public FieldType {
private:
    explicit Array(const Tuple *tuple_type) : tuple_type(tuple_type) {}

protected:
    Array() = default;
    friend struct nlohmann::adl_serializer<std::unique_ptr<Type>>;

public:
    static auto MakeArray(const Tuple *type, const ArrayLayout &layout,
                          const size_t &num_dimensions) -> const Array *;

    [[nodiscard]] auto to_string() const -> std::string override;
    void from_json(const nlohmann::json &json) override;
    void to_json(nlohmann::json *json) const override;

    ArrayLayout layout{};
    size_t num_dimensions{};
    const Tuple *tuple_type{};
};

}  // namespace type
}  // namespace dag

namespace nlohmann {

template <>
struct adl_serializer<dag::type::ArrayLayout> {
    static auto from_json(const nlohmann::json &json) -> dag::type::ArrayLayout;
    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json, dag::type::ArrayLayout layout);
};

}  // namespace nlohmann

#endif  // DAG_TYPE_ARRAY_HPP
