#ifndef UTILS_REGISTRY_HPP_
#define UTILS_REGISTRY_HPP_

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>

namespace utils {

template <typename Object, typename Key = std::string>
class Registry {
private:
    std::unordered_map<Key, std::unique_ptr<Object>> objects_;

public:
    using ObjectRange =
            boost::iterator_range<typename decltype(objects_)::const_iterator>;

    static auto Register(const Key &key, std::unique_ptr<Object> object)
            -> bool;
    static auto at(const Key &key) -> Object *;
    static auto objects() -> ObjectRange;
    static auto num_objects() -> size_t;

private:
    static auto instance() -> Registry *;
};

template <typename Object, typename Key>
auto Registry<Object, Key>::Register(const Key &key,
                                     std::unique_ptr<Object> object) -> bool {
    return instance()->objects_.emplace(key, std::move(object)).second;
}

template <typename Object, typename Key>
auto Registry<Object, Key>::at(const Key &key) -> Object * {
    const auto it = instance()->objects_.find(key);
    if (it != instance()->objects_.end()) {
        return it->second.get();
    }
    throw std::runtime_error("Unknown object " +
                             boost::lexical_cast<std::string>(key));
}

template <typename Object, typename Key>
auto Registry<Object, Key>::objects() -> ObjectRange {
    return boost::make_iterator_range(instance()->objects_.begin(),
                                      instance()->objects_.end());
}

template <typename Object, typename Key>
auto Registry<Object, Key>::num_objects() -> size_t {
    return instance()->objects_.size();
}

template <typename Object, typename Key>
auto Registry<Object, Key>::instance() -> Registry<Object, Key> * {
    static auto registry = std::make_unique<Registry<Object, Key>>();
    return registry.get();
}

}  // namespace utils

#endif  // UTILS_REGISTRY_HPP_
