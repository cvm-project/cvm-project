//
// Created by sabir on 26.08.17.
//

#ifndef DAG_TUPLEFIELD_H
#define DAG_TUPLEFIELD_H

#include <memory>
#include <set>
#include <typeinfo>
#include <utility>

#include <boost/polymorphic_pointer_cast.hpp>

#include "dag/type/field_type.hpp"

namespace dag {

namespace utils {
class FieldVisitor;
}  // namespace utils

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
    // NOLINTNEXTLINE modernize-pass-by-value
    explicit Field(const type::FieldType *field_type, const size_t &position)
        : field_type_(field_type), position_(position) {}

    Field() = delete;
    Field(const Field &field) = default;
    Field(Field &&field) = default;
    Field &operator=(const Field &rhs) = default;
    Field &operator=(Field &&rhs) = default;

    virtual ~Field();

    virtual void Accept(utils::FieldVisitor *visitor) = 0;

    size_t position() const;
    void set_position(size_t position);

    const type::FieldType *field_type() const;
    void set_field_type(const type::FieldType *field_type);

    const std::shared_ptr<AttributeId> &attribute_id() const;

    const std::set<FieldProperty> &properties() const;
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

    const FieldTypeClass *field_type() {
        return boost::polymorphic_pointer_downcast<const FieldTypeClass>(
                Field::field_type());
    }

    void inline Accept(utils::FieldVisitor *visitor) override;
};

}  // namespace collection
}  // namespace dag

#include "dag/utils/field_visitor.hpp"

#endif  // DAG_TUPLEFIELD_H
