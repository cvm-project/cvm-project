#include "utils/lib_path.hpp"

#include <boost/filesystem/path.hpp>

auto get_lib_path() -> boost::filesystem::path {
    auto *const lib_path = std::getenv("JITQPATH");
    if (lib_path == nullptr) {
        throw std::runtime_error(
                "JITQPATH is not defined, set it to your jitq installation "
                "path\n");
    }
    return boost::filesystem::path(lib_path);
}
