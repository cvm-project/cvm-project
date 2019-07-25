#ifndef RUNTIME_OPERATORS_TYPE_TRAITS_HPP
#define RUNTIME_OPERATORS_TYPE_TRAITS_HPP

namespace runtime {
namespace operators {

template <typename T>
struct TypeName {};

template <>
struct TypeName<int> {
    static constexpr const char *const kValue = "int";
};

template <>
struct TypeName<long> {
    static constexpr const char *const kValue = "long";
};

template <>
struct TypeName<float> {
    static constexpr const char *const kValue = "float";
};

template <>
struct TypeName<double> {
    static constexpr const char *const kValue = "double";
};

}  // namespace operators
}  // namespace runtime

#endif  // RUNTIME_OPERATORS_TYPE_TRAITS_HPP
