#ifndef CODE_GEN_CPP_OPERATORS_EXPANDPATTERNOPERATOR_H
#define CODE_GEN_CPP_OPERATORS_EXPANDPATTERNOPERATOR_H

#include "Utils.h"
#include "runtime/jit/operators/expand_pattern.hpp"
#include "runtime/jit/operators/optional.hpp"

template <class Tuple, class PatternUpstream, class ParameterUpstream>
class ExpandPatternOperator {
public:
    ExpandPatternOperator(PatternUpstream* const pattern_upstream,
                          ParameterUpstream* const parameter_upstream)
        : pattern_upstream_(pattern_upstream),
          parameter_upstream_(parameter_upstream) {}

    INLINE void open() {
        pattern_upstream_->open();
        const auto ret = pattern_upstream_->next();
        assert(ret);
        pattern_ = ret.value().v0;
        pattern_upstream_->close();

        parameter_upstream_->open();
    }

    INLINE Optional<Tuple> next() {
        auto const ret = parameter_upstream_->next();
        if (!ret) return {};

        return Tuple{
                runtime::operators::ExpandPattern(pattern_, ret.value().v0)};
    }

    INLINE void close() { parameter_upstream_->close(); }

private:
    PatternUpstream* const pattern_upstream_;
    ParameterUpstream* const parameter_upstream_;
    std::string pattern_;
};

template <class Tuple, class PatternUpstream, class ParameterUpstream>
ExpandPatternOperator<Tuple, PatternUpstream, ParameterUpstream>
makeExpandPatternOperator(PatternUpstream* const pattern_upstream,
                          ParameterUpstream* const parameter_upstream) {
    return ExpandPatternOperator<Tuple, PatternUpstream, ParameterUpstream>(
            pattern_upstream, parameter_upstream);
};

#endif  // CODE_GEN_CPP_OPERATORS_EXPANDPATTERNOPERATOR_H
