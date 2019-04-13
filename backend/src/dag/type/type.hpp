//
// Created by sabir on 13.06.18.
//

#ifndef CPP_DAG_TYPE_TYPE_HPP
#define CPP_DAG_TYPE_TYPE_HPP

#include <memory>
#include <string>

#include <boost/polymorphic_pointer_cast.hpp>

#include <nlohmann/json.hpp>

#include "utils/registry.hpp"
#include "utils/utils.h"

namespace dag {
namespace type {

struct Type {
public:
    virtual ~Type() = default;
    virtual std::string to_string() const = 0;

    Type(Type const &) = delete;
    Type(Type const &&) = delete;

    void operator=(Type const &) = delete;
    void operator=(Type const &&) = delete;

    virtual void from_json(const nlohmann::json &json) = 0;
    virtual void to_json(nlohmann::json *json) const = 0;

protected:
    Type() = default;
};

using TypeRegistry = ::utils::Registry<const Type>;
const Type *MakeType(std::unique_ptr<const Type> &&type);

}  // namespace type
}  // namespace dag

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<dag::type::Type>> {
    using Type = dag::type::Type;
    static std::unique_ptr<Type> from_json(const nlohmann::json &json);
};

template <typename DagType>
struct adl_serializer<raw_ptr<const DagType>> {
    static raw_ptr<const DagType> from_json(const nlohmann::json &json) {
        // Get empty type and use it for deserialization
        auto type = json.get<std::unique_ptr<dag::type::Type>>();
        type->from_json(json);

        // Look up unique type object in registry
        std::unique_ptr<const dag::type::Type> const_type(type.release());
        const auto ret_type = MakeType(std::move(const_type));

        // Return inside raw_ptr
        return make_raw(
                boost::polymorphic_pointer_downcast<const DagType>(ret_type));
    }
};

// NOLINTNEXTLINE google-runtime-references
void to_json(nlohmann::json &json, const dag::type::Type *type);

}  // namespace nlohmann

#endif  // CPP_DAG_TYPE_TYPE_HPP
