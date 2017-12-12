//
// Created by sabir on 23.07.17.
//

#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <string>
#include <vector>

using namespace std;

string snake_to_camel_string(const std::string &str);

string string_replace(const std::string &str, const std::string &from,
                      const std::string &to);
vector<string> split_string(const std::string &str,
                            const std::string &delimiter);
string get_lib_path();
std::string exec(const char *cmd);

#endif  // CPP_UTILS_H
