#include "dag/type/atomic.hpp"

#include <boost/polymorphic_pointer_cast.hpp>
#include <nlohmann/json.hpp>

using dag::type::Atomic;

namespace dag {
namespace type {

const Atomic *Atomic::MakeAtomic(const std::string &type) {
    std::unique_ptr<Atomic> ret(new Atomic());
    ret->type = type;
    return boost::polymorphic_pointer_downcast<const Atomic>(
            MakeType(std::move(ret)));
}

std::string Atomic::to_string() const { return type; }

void Atomic::from_json(const nlohmann::json &json) { type = json.at("type"); }

void Atomic::to_json(nlohmann::json *json) const {
    json->emplace("type", type);
}

}  // namespace type
}  // namespace dag
