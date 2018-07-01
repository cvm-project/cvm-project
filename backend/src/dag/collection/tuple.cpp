//
// Created by sabir on 22.05.18.
//

#include "dag/attribute_id.h"

#include <memory>

#include <json.hpp>

#include "array.hpp"
#include "atomic.hpp"
#include "tuple.hpp"
#include "utils/utils.h"

namespace dag {
namespace collection {

void Tuple::from_json(const nlohmann::json &json) {
    raw_ptr<const dag::type::Tuple> t = json;
    this->type = t.get();

    size_t pos = 0;
    for (auto &it : json) {
        auto field_type = this->type->field_types.at(pos);
        if (it.at("type") == "array") {
            this->fields.push_back(
                    std::make_unique<Array>(field_type, pos, it));
        } else {
            this->fields.push_back(std::make_unique<Atomic>(field_type, pos));
        }
        Field *field = this->fields.back().get();
        auto attribute_id = AttributeId::MakeAttributeId();
        attribute_id->AddField(field);
        pos++;
    }
}

}  // namespace collection
}  // namespace dag
