//
// Created by sabir on 18.07.17.
//

#ifndef CPP_OPTIONAL_H
#define CPP_OPTIONAL_H

#include <tuple>

using std::tuple;

template<typename T>
class Optional {
public:
    T value;
    bool initialized;

    Optional(T value) : value(value), initialized(1) {}

    Optional() : initialized(0) {}

    explicit operator bool() const { return (initialized); }

    operator T() const { return value; }

//    /**
//     * if matched here instead of above, T is a tuple type and we need to convert it to a struct
//     */
//    template<typename Z>
//    operator Z() const { return make_struct<Z>(value); }

private:

//    template<typename result_type, typename ...types, std::size_t ...indices>
//    result_type
//    make_struct(std::tuple<types...> t, std::index_sequence<indices...>) const
//    {
//        return {{std::get<indices>(t)...}};
//    }
//
//    template<typename result_type, typename ...types>
//    result_type
//    make_struct(std::tuple<types...> t) const
//    {
//        return make_struct<result_type, types...>(t, std::index_sequence_for<types...>{});
//    }

};


#endif //CPP_OPTIONAL_H
