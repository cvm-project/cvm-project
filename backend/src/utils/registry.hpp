#ifndef UTILS_REGISTRY_HPP_
#define UTILS_REGISTRY_HPP_

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <boost/range/iterator_range.hpp>

namespace utils {

template <typename Object, typename Key = std::string>
class Registry {
private:
    std::unordered_map<Key, std::unique_ptr<Object>> objects_;

public:
    using ObjectRange =
            boost::iterator_range<typename decltype(objects_)::const_iterator>;

    static bool Register(const std::string &name,
                         std::unique_ptr<Object> object);
    static Object *at(const std::string &name);
    static ObjectRange objects();

private:
    static Registry *instance();
};

template <typename Object, typename Key>
bool Registry<Object, Key>::Register(const std::string &name,
                                     std::unique_ptr<Object> object) {
    return instance()->objects_.emplace(name, std::move(object)).second;
}

template <typename Object, typename Key>
Object *Registry<Object, Key>::at(const std::string &name) {
    const auto it = instance()->objects_.find(name);
    if (it != instance()->objects_.end()) {
        return it->second.get();
    }
    throw std::runtime_error("Unknown object " + name);
}

template <typename Object, typename Key>
auto Registry<Object, Key>::objects() -> ObjectRange {
    return boost::make_iterator_range(instance()->objects_.begin(),
                                      instance()->objects_.end());
}

template <typename Object, typename Key>
Registry<Object, Key> *Registry<Object, Key>::instance() {
    static auto registry = std::make_unique<Registry<Object, Key>>();
    return registry.get();
}

}  // namespace utils

#endif  // UTILS_REGISTRY_HPP_
