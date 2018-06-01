//
// Created by sabir on 13.06.18.
//

#include "type.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace dag {
namespace type {

std::unordered_map<std::string, std::unique_ptr<const Type>>& Type::registry() {
    static std::unordered_map<std::string, std::unique_ptr<const Type>>
            registry;
    return registry;
}

}  // namespace type
}  // namespace dag
