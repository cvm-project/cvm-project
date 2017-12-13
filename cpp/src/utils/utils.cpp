//
// Created by sabir on 23.07.17.
//

#include "utils.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>

string snake_to_camel_string(const string &str) {
    string camelString = str;

    for (size_t x = 0; x < camelString.length() - 1; x++) {
        if (camelString[x] == '_') {
            char nextC = camelString[x + 1];
            nextC = nextC > 65 ? nextC - 32 : nextC;
            camelString.erase(x, 2);
            camelString.insert(x, 1, nextC);
        }
    }

    return camelString;
}

string string_replace(const string &str, const string &from, const string &to) {
    std::string ret = str;
    size_t index = 0;
    while (true) {
        /* Locate the substring to replace. */
        index = ret.find(from, index);
        if (index == std::string::npos) break;

        ret.replace(index, from.size(), to);
        index += to.size();
    }
    return ret;
}

vector<string> split_string(const string &str, const string &delimiter) {
    vector<string> list;
    std::string tmp = str;
    size_t pos = 0;
    string token;
    while ((pos = tmp.find(delimiter)) != string::npos) {
        token = tmp.substr(0, pos);
        list.push_back(token);
        tmp.erase(0, pos + delimiter.length());
    }
    list.push_back(tmp);
    return list;
}

string get_lib_path() {
    const char *const blazePath = std::getenv("BLAZEPATH");
    if (blazePath == nullptr) {
        std::cerr << "BLAZEPATH is not defined, set it to your blaze "
                     "installation path\n";
    }
    return string(blazePath);
}

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (feof(pipe.get()) == 0) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    return result;
}
