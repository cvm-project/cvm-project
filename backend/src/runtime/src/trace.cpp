#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

void Trace(const std::string &message) {
    static std::mutex trace_mutex;
    std::lock_guard<std::mutex> l(trace_mutex);
    std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count()
              << ": [" << std::this_thread::get_id() << "] " << message
              << std::endl;
}
