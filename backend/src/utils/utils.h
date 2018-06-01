//
// Created by sabir on 23.07.17.
//

#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <string>
#include <vector>

std::string snake_to_camel_string(const std::string &str);

std::string string_replace(const std::string &str, const std::string &from,
                           const std::string &to);
std::vector<std::string> split_string(const std::string &str,
                                      const std::string &delimiter);
std::string get_lib_path();
std::string exec(const char *cmd);

template <typename Pointee>
class raw_ptr {
public:
    //  raw_ptr() = default;
    explicit raw_ptr(Pointee *ptr) : ptr_(ptr) {}

    Pointee *operator->() { return ptr_; }

    Pointee &operator*() { return *ptr_; }

    Pointee *get() { return ptr_; }

private:
    Pointee *ptr_;
};

template <typename Pointee>
static raw_ptr<Pointee> make_raw(Pointee *ptr) {
    return raw_ptr<Pointee>(ptr);
}

#endif  // UTILS_UTILS_H
