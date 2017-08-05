//
// Created by sabir on 23.07.17.
//

#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <string>
#include <vector>

using namespace std;

string snake_to_camel_string(string str);

string string_replace(string str, string from, string to);
vector<string> split_string(string s, string delimiter);

#endif //CPP_UTILS_H
