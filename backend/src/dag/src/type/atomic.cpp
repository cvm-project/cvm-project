#include "dag/type/atomic.hpp"

#include <boost/polymorphic_pointer_cast.hpp>
#include <nlohmann/json.hpp>

using dag::type::Atomic;

namespace dag::type {

auto Atomic::MakeAtomic(const std::string &type) -> const Atomic * {
    std::unique_ptr<Atomic> ret(new Atomic());
    ret->type = type;
    return boost::polymorphic_pointer_downcast<const Atomic>(
            MakeType(std::move(ret)));
}

auto Atomic::to_string() const -> std::string { return type; }

void Atomic::from_json(const nlohmann::json &json) { type = json.at("type"); }

void Atomic::to_json(nlohmann::json *json) const {
    json->emplace("type", type);
}

}  // namespace dag::type
