#include <iostream>
#include <string>
#include <tuple>

template<typename... T>
std::string to_string(const std::tuple<T...> &tup);