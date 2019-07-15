//
// Created by sabir on 11.05.18.
//

#ifndef DAG_TYPE_ARRAY_HPP
#define DAG_TYPE_ARRAY_HPP

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "field_type.hpp"
#include "utils/utils.h"

namespace dag {
namespace type {

struct Tuple;

enum ArrayLayout { kC, kF };
std::string to_string(const ArrayLayout &layout);

struct Array : public FieldType {
private:
    explicit Array(const Tuple *tuple_type) : tuple_type(tuple_type) {}

protected:
    Array() = default;
    friend struct nlohmann::adl_serializer<std::unique_ptr<Type>>;

public:
    static const Array *MakeArray(const Tuple *type, const ArrayLayout &layout,
                                  const size_t &num_dimensions);

    std::string to_string() const override;
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
    static dag::type::ArrayLayout from_json(const nlohmann::json &json);
    // NOLINTNEXTLINE(google-runtime-references)
    static void to_json(nlohmann::json &json, dag::type::ArrayLayout layout);
};

}  // namespace nlohmann

#endif  // DAG_TYPE_ARRAY_HPP
