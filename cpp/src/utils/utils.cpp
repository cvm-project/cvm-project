//
// Created by sabir on 23.07.17.
//

#include <algorithm>
#include <iostream>
#include <array>
#include <memory>
#include "utils.h"

string snake_to_camel_string(string str) {
    string tempString = "";
    string camelString = "";

    camelString = str;

    for (size_t x = 0; x < camelString.length() - 1; x++) {
        if (camelString[x] == '_') {
            char nextC = camelString[x + 1];
            nextC = (char) (nextC > 65 ? nextC - 32 : nextC);
            camelString.erase(x, 2);
            camelString.insert(x, 1, nextC);
        }
    }


    return camelString;
}

string string_replace(string str, string from, string to) {
    size_t index = 0;
    while (true) {
        /* Locate the substring to replace. */
        index = str.find(from, index);
        if (index == std::string::npos) break;

        str.replace(index, from.size(), to);
        index += to.size();
    }
    return str;
}

vector<string> split_string(string s, string delimiter) {
    vector<string> list;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}

string get_lib_path() {
    const char *blazePath;
    if (!(blazePath = std::getenv("BLAZEPATH")))
        std::cerr << "BLAZEPATH is not defined, set it to your blaze installation path\n";
    return string(blazePath);
}

std::string exec(const char *cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    return result;
}
