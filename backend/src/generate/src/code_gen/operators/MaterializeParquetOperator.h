#ifndef CODE_GEN_OPERATORS_MATERIALIZEPARQUETOPERATOR_H
#define CODE_GEN_OPERATORS_MATERIALIZEPARQUETOPERATOR_H

#include <memory>
#include <string>
#include <vector>

#include "TupleToValueOperator.h"
#include "Utils.h"
#include "runtime/jit/operators/materialize_parquet.hpp"
#include "runtime/jit/operators/optional.hpp"
#include "runtime/jit/operators/type_traits.hpp"
#include "runtime/jit/values/none.hpp"

template <class OutputTuple>
class MaterializeParquetOperator {
public:
    MaterializeParquetOperator(
            std::unique_ptr<runtime::operators::ValueOperator> upstream)
        : upstream_(std::move(upstream)) {}

    INLINE void open() { upstream_->open(); }

    INLINE Optional<OutputTuple> next() {
        const auto ret = upstream_->next();
        if (dynamic_cast<const runtime::values::None *>(ret.get()) != nullptr) {
            return {};
        }
        return ValueToTuple<OutputTuple>(ret);
    }

    INLINE void close() { upstream_->close(); }

private:
    std::unique_ptr<runtime::operators::ValueOperator> upstream_;
};

template <class OutputTuple, class MainUpstream, class ConfUpstream,
          class... InputTypes>
auto makeMaterializeParquetOperator(MainUpstream *const main_upstream,
                                    ConfUpstream *const conf_upstream,
                                    std::vector<std::string> column_names,
                                    std::tuple<InputTypes...> /*tag*/) {
    static constexpr const char *const kColumnTypes[sizeof...(InputTypes)] = {
            runtime::operators::TypeName<decltype(
                    std::declval<InputTypes>().data[0].v0)>::kValue...};

    std::vector<std::string> column_types(std::begin(kColumnTypes),
                                          std::end(kColumnTypes));

    return MaterializeParquetOperator<OutputTuple>(
            runtime::operators::MakeMaterializeParquetOperator(
                    std::make_unique<TupleToValueOperator<MainUpstream>>(
                            main_upstream),
                    std::make_unique<TupleToValueOperator<ConfUpstream>>(
                            conf_upstream),
                    std::move(column_types), std::move(column_names)));
}

template <class OutputTuple, class MainUpstream, class ConfUpstream>
auto makeMaterializeParquetOperator(MainUpstream *const main_upstream,
                                    ConfUpstream *const conf_upstream,
                                    std::vector<std::string> column_names) {
    using InputTuple = decltype(main_upstream->next().value());
    using StdInputTuple = decltype(TupleToStdTuple(std::declval<InputTuple>()));
    return makeMaterializeParquetOperator<OutputTuple, MainUpstream,
                                          ConfUpstream>(
            main_upstream, conf_upstream, std::move(column_names),
            StdInputTuple());
};

#endif  // CODE_GEN_OPERATORS_MATERIALIZEPARQUETOPERATOR_H
