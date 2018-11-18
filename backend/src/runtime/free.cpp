#include "free.hpp"

#include <boost/mpl/list.hpp>

#include "utils/visitor.hpp"
#include "values/array.hpp"
#include "values/json_parsing.hpp"
#include "values/value.hpp"

namespace runtime {

struct FreeVisitor : public Visitor<FreeVisitor, const values::Value,
                                    boost::mpl::list<            //
                                            const values::Array  //
                                            >::type> {
    void operator()(const values::Array* const v) { free(v->data); }
};

void FreeValues(const std::string& values_str) {
    const auto values = values::ConvertFromJsonString(values_str.c_str());
    for (auto const& v : values) {
        FreeVisitor().Visit(v.get());
    }
}

}  // namespace runtime
