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
        std::shared_ptr<Field> field;
        auto field_type = this->type->field_types.at(pos);
        if (it.at("type") == "array") {
            field = std::make_shared<Array>(field_type, it, pos);
        } else {
            field = std::make_shared<Atomic>(field_type, pos);
        }
        auto attribute_id = AttributeId::MakeAttributeId();
        attribute_id->AddField(field.get());
        this->fields.push_back(field);
        pos++;
    }
}

}  // namespace collection
}  // namespace dag
