#ifndef OPTIMIZE_DAG_TRANSFORMATION_HPP
#define OPTIMIZE_DAG_TRANSFORMATION_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/range/iterator_range.hpp>

class DAG;

namespace optimize {

class DagTransformation {
public:
    virtual ~DagTransformation() = default;
    virtual void Run(DAG *dag) const = 0;
    virtual std::string name() const = 0;
};

class DagTransformationRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<const DagTransformation>>
            transformations_;

public:
    using TransformationRange =
            boost::iterator_range<decltype(transformations_)::const_iterator>;

    static bool RegisterTransformation(const std::string &name,
                                       const DagTransformation *transformation);
    static const DagTransformation *transformation(const std::string &name);
    static TransformationRange transformations();

private:
    static DagTransformationRegistry *instance();
};

void LoadDagTransformations();

}  // namespace optimize

#endif  // OPTIMIZE_DAG_TRANSFORMATION_HPP
