#include "dag_transformation.hpp"

#include <stdexcept>

#include <boost/range/iterator_range.hpp>

#include "add_always_inline.hpp"
#include "assert_correct_open_next_close.hpp"
#include "attribute_id_tracking.hpp"
#include "canonicalize.hpp"
#include "create_pipelines.hpp"
#include "determine_sortedness.hpp"
#include "grouped_reduce_by_key.hpp"
#include "materialize_multiple_reads.hpp"
#include "parallelize.hpp"
#include "simple_predicate_move_around.hpp"
#include "type_inference.hpp"

namespace optimize {

bool DagTransformationRegistry::RegisterTransformation(
        const std::string &name,
        const DagTransformation *const transformation) {
    std::unique_ptr<const DagTransformation> transformation_ptr(transformation);
    return instance()
            ->transformations_.emplace(name, std::move(transformation_ptr))
            .second;
}

const DagTransformation *DagTransformationRegistry::transformation(
        const std::string &name) {
    const auto it = instance()->transformations_.find(name);
    if (it != instance()->transformations_.end()) {
        return it->second.get();
    }
    throw std::runtime_error("Unknown transformation " + name);
}

auto DagTransformationRegistry::transformations() -> TransformationRange {
    return boost::make_iterator_range(instance()->transformations_.begin(),
                                      instance()->transformations_.end());
}

DagTransformationRegistry *DagTransformationRegistry::instance() {
    static auto registry = std::make_unique<DagTransformationRegistry>();
    return registry.get();
}

void LoadDagTransformations() {
    static bool has_loaded = false;
    if (has_loaded) return;

    auto const Register = DagTransformationRegistry::RegisterTransformation;
    auto const RegisterDefault = [&](auto const t) { Register(t->name(), t); };

    RegisterDefault(new AssertCorrectOpenNextClose());
    RegisterDefault(new AttributeIdTracking());
    RegisterDefault(new Canonicalize());
    RegisterDefault(new CreatePipelines());
    RegisterDefault(new DetermineSortedness());
    RegisterDefault(new GroupedReduceByKey());
    RegisterDefault(new MaterializeMultipleReads());
    RegisterDefault(new Parallelize());
    RegisterDefault(new SimplePredicateMoveAround());
    RegisterDefault(new TypeInference());
    RegisterDefault(new AddAlwaysInline());
    Register("type_check", new TypeInference(true));

    has_loaded = true;
}

}  // namespace optimize
