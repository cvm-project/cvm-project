#include "runtime/memory/values.hpp"

#include <boost/mpl/list.hpp>

#include "runtime/jit/memory/shared_pointer.hpp"
#include "runtime/jit/values/array.hpp"
#include "runtime/jit/values/json_parsing.hpp"
#include "runtime/jit/values/tuple.hpp"
#include "runtime/jit/values/value.hpp"
#include "utils/visitor.hpp"

namespace runtime {
namespace memory {

template <uint64_t kOperation(RefCounter*)>
struct ValueVisitor
    : public Visitor<ValueVisitor<kOperation>, const values::Value,
                     boost::mpl::list<             //
                             const values::Tuple,  //
                             const values::Array   //
                             >::type> {
    void operator()(const values::Array* const v) {
        kOperation(v->data.ref_counter());
    }

    void operator()(const values::Tuple* const v) {
        for (const auto& f : v->fields) {
            ValueVisitor<kOperation>().Visit(f.get());
        }
    }
};

void Decrement(const std::string& values) {
    Decrement(values::ConvertFromJsonString(values.c_str()));
}

void Decrement(const values::VectorOfValues& values) {
    for (auto const& v : values) {
        ValueVisitor<Decrement>().Visit(v.get());
    }
}

void Increment(const std::string& values) {
    Increment(values::ConvertFromJsonString(values.c_str()));
}

void Increment(const values::VectorOfValues& values) {
    for (auto const& v : values) {
        ValueVisitor<Increment>().Visit(v.get());
    }
}

}  // namespace memory
}  // namespace runtime
