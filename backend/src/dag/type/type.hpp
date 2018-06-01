//
// Created by sabir on 13.06.18.
//

#ifndef CPP_DAG_TYPE_TYPE_HPP
#define CPP_DAG_TYPE_TYPE_HPP

#include <memory>
#include <string>
#include <unordered_map>

namespace dag {
namespace type {

struct Type {
public:
    virtual ~Type() = default;
    virtual std::string to_string() const = 0;

    Type(Type const &) = delete;
    Type(Type const &&) = delete;

    void operator=(Type const &) = delete;
    void operator=(Type const &&) = delete;

protected:
    Type() = default;

    static std::unordered_map<std::string, std::unique_ptr<const Type>>
            &registry();
};

}  // namespace type
}  // namespace dag
#endif  // CPP_DAG_TYPE_TYPE_HPP
