//
// Created by sabir on 23.07.17.
//

#include "utils.h"

#include <boost/filesystem/path.hpp>

boost::filesystem::path get_lib_path() {
    const auto lib_path = std::getenv("JITQPATH");
    if (lib_path == nullptr) {
        throw std::runtime_error(
                "JITQPATH is not defined, set it to your jitq installation "
                "path\n");
    }
    return boost::filesystem::path(lib_path);
}
