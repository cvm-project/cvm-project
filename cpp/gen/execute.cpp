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
typedef tuple<double, double, double> tuple_1;

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


extern "C" void execute() {
	
/**RangeSourceOperator**/
	auto op_0 = makeRangeSourceOperator<tuple_0>(1.000000, 12000.000000, 1.200000);
	
	
/**MapOperator**/
	class mapOperatorFunction0 {
	public:
		auto operator()(tuple_0 t) {
			return call(map_operator_function_0, t);
		}
	};
	auto op_1 = makeMapOperator<tuple_1>(&op_0, mapOperatorFunction0());
	
	op_1.open();
	while (auto res = op_1.next()) {
		cout << to_string(res.value) << endl;
	}
}
