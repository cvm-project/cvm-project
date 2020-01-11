//
// Created by sabir on 11.05.18.
//

#ifndef DAG_TYPE_ATOMIC_HPP
#define DAG_TYPE_ATOMIC_HPP

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "field_type.hpp"

namespace dag {
namespace type {

struct Atomic : public FieldType {
protected:
    Atomic() = default;
    friend struct nlohmann::adl_serializer<std::unique_ptr<Type>>;

public:
    static auto MakeAtomic(const std::string &type) -> const Atomic *;

    auto to_string() const -> std::string override;
    void from_json(const nlohmann::json &json) override;
    void to_json(nlohmann::json *json) const override;

    std::string type = "";
};

}  // namespace type
}  // namespace dag

#endif  // DAG_TYPE_ATOMIC_HPP
