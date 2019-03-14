//
// Created by sabir on 23.07.17.
//

#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>

std::string snake_to_camel_string(const std::string &str);

std::string string_replace(const std::string &str, const std::string &from,
                           const std::string &to);
std::vector<std::string> split_string(const std::string &str,
                                      const std::string &delimiter);
boost::filesystem::path get_lib_path();

template <typename Pointee>
class raw_ptr {
public:
    explicit raw_ptr(Pointee *const ptr) : ptr_(ptr) {}

    raw_ptr() = default;
    raw_ptr(const raw_ptr &other) = default;
    raw_ptr(raw_ptr &&other) noexcept = default;
    raw_ptr &operator=(const raw_ptr &other) = default;
    raw_ptr &operator=(raw_ptr &&other) noexcept = default;
    ~raw_ptr() = default;

    Pointee *operator->() const { return ptr_; }
    Pointee &operator*() const { return *ptr_; }
    Pointee *get() const { return ptr_; }

private:
    Pointee *ptr_;
};

template <typename Pointee>
static raw_ptr<Pointee> make_raw(Pointee *ptr) {
    return raw_ptr<Pointee>(ptr);
}

#endif  // UTILS_UTILS_H
