//
// Created by sabir on 16.05.18.
//

#include "atomic.hpp"

#include <boost/polymorphic_pointer_cast.hpp>

#include <json.hpp>

#include "dag/collection/atomic.hpp"
#include "utils/utils.h"

using dag::type::Atomic;

namespace dag {
namespace type {

const Atomic *Atomic::MakeAtomic(const std::string &type) {
    std::unique_ptr<Atomic> ret(new Atomic());
    ret->type = type;

    const auto str = ret->to_string();
    TypeRegistry::Register(str, std::move(ret));
    return boost::polymorphic_pointer_downcast<const Atomic>(
            TypeRegistry::at(str));
}

std::string Atomic::to_string() const { return type; }

void Atomic::to_json(nlohmann::json *json) const {
    json->emplace("type", type);
}

}  // namespace type
}  // namespace dag

raw_ptr<const Atomic> nlohmann::adl_serializer<
        raw_ptr<const Atomic>>::from_json(const nlohmann::json &json) {
    return raw_ptr<const Atomic>(Atomic::MakeAtomic(json.at("type")));
}
