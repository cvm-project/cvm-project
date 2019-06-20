#include "free.hpp"

#include <boost/mpl/list.hpp>

#include "runtime/values/array.hpp"
#include "runtime/values/json_parsing.hpp"
#include "runtime/values/tuple.hpp"
#include "runtime/values/value.hpp"
#include "utils/visitor.hpp"

namespace runtime {
namespace memory {

struct FreeVisitor : public Visitor<FreeVisitor, const values::Value,
                                    boost::mpl::list<             //
                                            const values::Tuple,  //
                                            const values::Array   //
                                            >::type> {
    void operator()(const values::Array* const v) { free(v->data); }

    void operator()(const values::Tuple* const v) {
        for (const auto& f : v->fields) {
            FreeVisitor().Visit(f.get());
        }
    }
};

void FreeValues(const std::string& values) {
    FreeValues(values::ConvertFromJsonString(values.c_str()));
}

void FreeValues(const values::VectorOfValues& values) {
    for (auto const& v : values) {
        FreeVisitor().Visit(v.get());
    }
}

}  // namespace memory
}  // namespace runtime
