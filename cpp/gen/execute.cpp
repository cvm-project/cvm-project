/**
 * Auto-generated execution plan
 */

#include "operators/MapOperator.h"
#include "operators/RangeSourceOperator.h"
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


extern "C" tuple_1 *execute() {
	
/**RangeSourceOperator**/
	auto op_0 = makeRangeSourceOperator<tuple_0>(1.000000, 10.000000, 1.200000);
	
	
/**MapOperator**/
	class mapOperatorFunction0 {
	public:
		auto operator()(tuple_0 t) {
			return call(map_operator_function_0, t);
		}
	};
	auto op_1 = makeMapOperator<tuple_1>(&op_0, mapOperatorFunction0());
	
	    size_t allocatedSize = 2;
    size_t resSize = 0;
tuple_1     *result = (tuple_1 *) malloc(sizeof(tuple_1) * allocatedSize);
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
