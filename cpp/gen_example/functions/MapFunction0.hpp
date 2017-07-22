//
// Created by sabir on 18.07.17.
//

#ifndef CPP_MAPFUNCTION0_H
#define CPP_MAPFUNCTION0_H


#include <tuple>
#include <utility>

typedef std::tuple<long, long> rettype;
typedef std::tuple<long, long, long, long> inputtype;

extern "C" {
rettype llvm_map_function0(long, long, long, long);
}

namespace impl {
    template<typename Function, typename... Types, size_t... Indexes>
    auto call_impl(const Function &f, const std::tuple<Types...> &t,
                   const std::integer_sequence<size_t, Indexes...> &) {
        return f(std::get<Indexes>(t)...);
    }

}  // namespace impl

template<typename Function, typename... Types>
auto call(const Function &f, const std::tuple<Types...> &t) {
    return impl::call_impl(f, t, std::index_sequence_for<Types...>());
}

class MapFunction0 {
public:
    rettype operator()(inputtype t) {
        //flatten arguments
        auto res = call(llvm_map_function0, t);
        DEBUG_PRINT(std::get<0>(res));
        return res;
    }
};

#endif //CPP_MAPFUNCTION0_H
