#include "trace.hpp"

#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include <boost/fiber/mutex.hpp>
#include <boost/fiber/operations.hpp>

const char* const alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

class ThreadNames {
private:
    std::map<std::thread::id, std::string> names_{};
    std::mutex mtx_{};

public:
    ThreadNames() = default;

    auto lookup() -> std::string {
        std::unique_lock<std::mutex> lk(mtx_);
        auto this_id(std::this_thread::get_id());
        auto found = names_.find(this_id);
        if (found != names_.end()) {
            return found->second;
        }
        BOOST_ASSERT(names_.size() < 26 * 26);
        auto const name = std::string() + alpha[names_.size() / 26] +
                          alpha[names_.size() % 26];
        names_[this_id] = name;
        return name;
    }
};

auto thread_names() -> ThreadNames& {
    static ThreadNames singleton;
    return singleton;
}

class FiberNames {
private:
    std::map<boost::fibers::fiber::id, std::string> names_{};
    unsigned next_{0};
    boost::fibers::mutex mtx_{};

public:
    FiberNames() = default;

    auto lookup() -> std::string {
        std::unique_lock<boost::fibers::mutex> lk(mtx_);
        auto this_id(boost::this_fiber::get_id());
        auto found = names_.find(this_id);
        if (found != names_.end()) {
            return found->second;
        }
        std::ostringstream out;
        // Bake into the fiber's name the thread name on which we first
        // lookup() its ID, to be able to spot when a fiber hops between
        // threads.
        out << thread_names().lookup() << next_++;
        std::string name(out.str());
        names_[this_id] = name;
        return name;
    }
};

auto fiber_names() -> FiberNames& {
    static FiberNames singleton;
    return singleton;
}

namespace runtime::fibers {

auto this_thread_name() -> std::string { return thread_names().lookup(); }

auto this_fiber_name() -> std::string { return fiber_names().lookup(); }

auto tag() -> std::string {
    std::ostringstream out;
    out << "Thread " << thread_names().lookup() << ": " << std::setw(4)
        << fiber_names().lookup() << std::setw(0);
    return out.str();
}

namespace detail {

void print_impl(std::ostream& out) { out << '\n'; }

}  // namespace detail

}  // namespace runtime::fibers
