/**
 * Auto-generated execution plan
 */

#include "operators/MapOperator.h"
#include "operators/RangeSourceOperator.h"
#include "utils/tuple_to_string.cpp"
#include "utils/debug_print.h"
#include "tuples.h"
#include <tuple>
#include <vector>

using namespace std;


extern "C" {
tuple_1 map_operator_function_0(tuple_0);
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

template<typename Function, typename Type>
auto call(const Function &f, const Type &t) {
    return f(t);
}


extern "C" {
result_type *execute() {

/**RangeSourceOperator**/
    auto op_0 = makeRangeSourceOperator<tuple_0>(0, 10, 1);


/**MapOperator**/
    class mapOperatorFunction0 {
    public:
        auto operator()(tuple_0 t) {
            return call(map_operator_function_0, t);
        }
    };
    auto op_1 = makeMapOperator<tuple_1>(&op_0, mapOperatorFunction0());

    op_1.open();

    size_t allocatedSize = 2;
    size_t resSize = 0;
    tuple_1 *result = (tuple_1 *) malloc(sizeof(tuple_1) * allocatedSize);
    while (auto res = op_1.next()) {
        if (allocatedSize <= resSize) {
            allocatedSize *= 2;
            result = (tuple_1 *) realloc(result, sizeof(tuple_1) * allocatedSize);
        }
        result[resSize] = res.value;
        resSize++;
    }
    result_type *ret = (result_type *) malloc(sizeof(result_type));
    ret->data = result;
    ret->size = resSize;
    DEBUG_PRINT("returning the result");
//    DEBUG_PRINT(ret->data[0].v1);
//    DEBUG_PRINT(ret->data[0].v2);
//    DEBUG_PRINT(ret->data[0].v3);
//    DEBUG_PRINT(ret->data[1].v1);
//    DEBUG_PRINT(ret->data[1].v2);
//    DEBUG_PRINT(ret->data[1].v3);
//    DEBUG_PRINT(ret->data[2].v1);
//    DEBUG_PRINT(ret->data[2].v2);
//    DEBUG_PRINT(ret->data[2].v3);
    return ret;
}

void free_result(result_type *ptr) {
    free(ptr->data);
    free(ptr);
    DEBUG_PRINT("freeing the result memory");
}

}