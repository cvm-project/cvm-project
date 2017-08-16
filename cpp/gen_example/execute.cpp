/**
 * Auto-generated execution plan
 */

#include "operators/MapOperator.h"
#include "operators/CollectionSourceOperator.h"
#include "utils/tuple_to_string.cpp"
#include <tuple>
#include <vector>


using namespace std;
typedef double tuple_0;
typedef struct {
    double v0;
    double v1;
} tuple_1;

typedef struct {
    unsigned int size;
    tuple_1 *data;
} result_type;

extern "C" {
INLINE tuple_1 map_operator_function_0(tuple_0);
}

namespace impl {
    template<typename Function, typename... Types, size_t... Indexes>
    INLINE auto call_impl(const Function &f, const std::tuple<Types...> &t,
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
size_t execute(void *input_ptr, size_t input_size) {
    auto op_0 = makeCollectionSourceOperator((tuple_0*)input_ptr, input_size);

    /**MapOperator**/
    class mapOperatorFunction0 {
    public:
        auto operator()(tuple_0 t) {
            return call(map_operator_function_0, t);
        }
    };

    auto op_1 = makeMapOperator<tuple_1>(&op_0, mapOperatorFunction0());


    /**counting the result**/
    op_1.open();
    size_t tuple_count = 0;
    while (auto res = op_1.next()) {
        DEBUG_PRINT("here " + to_string(res.value.v1));
        tuple_count++;
    }
    op_1.close();
    DEBUG_PRINT("there " + to_string(tuple_count));
    return tuple_count;
}
void free_result(result_type *ptr) {
    free(ptr->data);
    free(ptr);
    DEBUG_PRINT("freeing the result memory");
}
}
