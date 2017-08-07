#include <iostream>
#include <string>
#include <tuple>

const std::string SEPARATOR = ", ";

template<typename T, T...>
struct integer_sequence {
};

template<std::size_t N, std::size_t... I>
struct gen_indices : gen_indices<(N - 1), (N - 1), I...> {
};
template<std::size_t... I>
struct gen_indices<0, I...> : integer_sequence<std::size_t, I...> {
};

template<typename H>
std::string &to_string_impl(std::string &s, H &&h) {
    using std::to_string;
    s += to_string(std::forward<H>(h));
    return s;
}

template<typename H, typename... T>
std::string &to_string_impl(std::string &s, H &&h, T &&... t) {
    using std::to_string;
    s += to_string(std::forward<H>(h)) + SEPARATOR;
    return to_string_impl(s, std::forward<T>(t)...);
}

template<typename... T, std::size_t... I>
std::string to_string(const std::tuple<T...> &tup, integer_sequence<std::size_t, I...>) {
    std::string result;
    int ctx[] = {(to_string_impl(result, std::get<I>(tup)...), 0), 0};
    (void) ctx;
    return result;
}

template<typename... T>
std::string to_string(const std::tuple<T...> &tup) {
    return to_string(tup, gen_indices<sizeof...(T)>{});
}

template<class T>
std::string to_string(const T v) {
    return std::to_string(v);
}