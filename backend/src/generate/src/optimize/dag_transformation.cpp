#include "dag_transformation.hpp"

#include <stdexcept>

#include <boost/range/iterator_range.hpp>

#include "add_always_inline.hpp"
#include "assert_correct_open_next_close.hpp"
#include "attribute_id_tracking.hpp"
#include "canonicalize.hpp"
#include "code_gen.hpp"
#include "create_pipelines.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"
#include "materialize_multiple_reads.hpp"
#include "parallelize.hpp"
#include "parallelize_omp.hpp"
#include "simple_predicate_move_around.hpp"
#include "type_inference.hpp"

namespace optimize {

void LoadDagTransformations() {
    static bool has_loaded = false;
    if (has_loaded) return;

    auto const Register = DagTransformationRegistry::Register;
    auto const RegisterDefault = [&](auto &&t) {
        Register(t->name(), std::forward<decltype(t)>(t));
    };

    RegisterDefault(std::make_unique<AssertCorrectOpenNextClose>());
    RegisterDefault(std::make_unique<AttributeIdTracking>());
    RegisterDefault(std::make_unique<Canonicalize>());
    RegisterDefault(std::make_unique<CodeGen>());
    RegisterDefault(std::make_unique<CreatePipelines>());
    RegisterDefault(std::make_unique<DetermineSortedness>());
    RegisterDefault(std::make_unique<GroupedReduceByKey>());
    RegisterDefault(std::make_unique<MaterializeMultipleReads>());
    RegisterDefault(std::make_unique<Parallelize>());
    RegisterDefault(std::make_unique<ParallelizeOmp>());
    RegisterDefault(std::make_unique<SimplePredicateMoveAround>());
    RegisterDefault(std::make_unique<TypeInference>());
    RegisterDefault(std::make_unique<AddAlwaysInline>());
    Register("type_check", std::make_unique<TypeInference>(true));

    has_loaded = true;
}

}  // namespace optimize
