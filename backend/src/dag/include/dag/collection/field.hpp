#ifndef DAG_COLLECTION_FIELD_HPP
#define DAG_COLLECTION_FIELD_HPP

#include <memory>
#include <set>
#include <typeinfo>
#include <utility>

#include <boost/polymorphic_pointer_cast.hpp>

#include "dag/type/field_type.hpp"

namespace dag {

class AttributeId;

namespace collection {

/**
 * Properties specific for each field
 * Field is identified by its position within the tuple
 */
enum FieldProperty { FL_GROUPED, FL_SORTED, FL_UNIQUE };

class Field {
    friend AttributeId;

public:
    // NOLINTNEXTLINE(modernize-pass-by-value)
    explicit Field(const type::FieldType *field_type, const size_t &position)
        : field_type_(field_type), position_(position) {}

    Field() = delete;
    Field(const Field &field) = default;
    Field(Field &&field) = default;
    auto operator=(const Field &rhs) -> Field & = default;
    auto operator=(Field &&rhs) -> Field & = default;

    virtual ~Field();

    auto position() const -> size_t;
    void set_position(size_t position);

    auto field_type() const -> const type::FieldType *;
    void set_field_type(const type::FieldType *field_type);

    auto attribute_id() const -> const std::shared_ptr<AttributeId> &;

    auto properties() const -> const std::set<FieldProperty> &;
    void AddProperty(const FieldProperty &fieldProperty);
    void CopyProperties(const Field &other);
    void RemoveProperty(const FieldProperty &fieldProperty);
    void ClearProperties();

private:
    std::shared_ptr<AttributeId> attribute_id_;
    size_t position_;
    std::set<FieldProperty> properties_;
    const type::FieldType *field_type_;
};

template <class FieldClass, class FieldTypeClass>
class FieldBase : public Field {
public:
    explicit FieldBase(const type::FieldType *field_type_,
                       const size_t &position)
        : Field(field_type_, position) {}

    auto field_type() -> const FieldTypeClass * {
        return boost::polymorphic_pointer_downcast<const FieldTypeClass>(
                Field::field_type());
    }
};

}  // namespace collection
}  // namespace dag

#endif  // DAG_COLLECTION_FIELD_HPP
