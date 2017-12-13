//
// Created by sabir on 23.07.17.
//

#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <string>
#include <vector>

std::string snake_to_camel_string(const std::string &str);

std::string string_replace(const std::string &str, const std::string &from,
                           const std::string &to);
std::vector<std::string> split_string(const std::string &str,
                                      const std::string &delimiter);
std::string get_lib_path();
std::string exec(const char *cmd);

#endif  // CPP_UTILS_H
