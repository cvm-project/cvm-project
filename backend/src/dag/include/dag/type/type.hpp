#ifndef DAG_TYPE_TYPE_HPP
#define DAG_TYPE_TYPE_HPP

#include <memory>
#include <string>

#include <boost/polymorphic_pointer_cast.hpp>
#include <nlohmann/json.hpp>

#include "utils/raw_ptr.hpp"
#include "utils/registry.hpp"

namespace dag {
namespace type {

struct Type {
public:
    virtual ~Type() = default;
    virtual auto to_string() const -> std::string = 0;

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
auto MakeType(std::unique_ptr<const Type> &&type) -> const Type *;

}  // namespace type
}  // namespace dag

namespace nlohmann {

template <>
struct adl_serializer<std::unique_ptr<dag::type::Type>> {
    using Type = dag::type::Type;
    static auto from_json(const nlohmann::json &json) -> std::unique_ptr<Type>;
};

template <typename DagType>
struct adl_serializer<raw_ptr<const DagType>> {
    static auto from_json(const nlohmann::json &json)
            -> raw_ptr<const DagType> {
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

// NOLINTNEXTLINE(google-runtime-references)
void to_json(nlohmann::json &json, const dag::type::Type *type);

}  // namespace nlohmann

#endif  // DAG_TYPE_TYPE_HPP
