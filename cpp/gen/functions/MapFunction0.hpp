//
// Created by sabir on 18.07.17.
//

#ifndef CPP_MAPFUNCTION0_H
#define CPP_MAPFUNCTION0_H


#include <tuple>
#include <utility>
#include <cstdio>

typedef std::tuple<double, double> rettype;
typedef std::tuple<long, double, double> inputtype;

rettype llvm_map_function0(long, double, double);


namespace impl {
    template<typename Function, typename... Types, size_t... Indexes>
    rettype call_impl(const Function &f, const std::tuple<Types...> &t,
                      const std::integer_sequence<size_t, Indexes...> &) {
        return f(std::get<Indexes>(t)...);
    }

}  // namespace impl

template<typename Function, typename... Types>
rettype call(const Function &f, const std::tuple<Types...> &t) {
    return impl::call_impl(f, t, std::index_sequence_for<Types...>());
}

class MapFunction0 {
public:
    rettype operator()(inputtype t) {
        //flatten arguments
        return call(llvm_map_function0, t);
    }
};

#endif //CPP_MAPFUNCTION0_H
