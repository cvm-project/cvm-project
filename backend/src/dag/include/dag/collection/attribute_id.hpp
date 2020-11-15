#ifndef DAG_COLLECTION_ATTRIBUTE_ID_HPP
#define DAG_COLLECTION_ATTRIBUTE_ID_HPP

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "field.hpp"

namespace dag {

/**
 * part of the schema
 */
class AttributeId : public std::enable_shared_from_this<AttributeId> {
public:
    static auto MakeAttributeId() -> std::shared_ptr<AttributeId>;

    void AddField(collection::Field *field);
    void MoveFields(const std::shared_ptr<AttributeId> &other);
    void RemoveField(collection::Field *field);

    auto operator==(const AttributeId &other) const -> bool {
        return id_ == other.id_;
    }

    auto operator<(const AttributeId &other) const -> bool {
        return id_ < other.id_;
    }

    auto name() const -> std::string { return "c" + std::to_string(id_); }

private:
    static auto next_global_id() -> size_t;

    AttributeId() : id_(next_global_id()) {}

    const size_t id_;
    std::unordered_set<collection::Field *> fields_{};
};
}  // namespace dag

#endif  // DAG_COLLECTION_ATTRIBUTE_ID_HPP
