//
// Created by sabir on 23.07.17.
//

#include <algorithm>
#include <iostream>
#include "utils.h"

string snake_to_camel_string(string str) {
    string tempString = "";
    string camelString = "";

    camelString = str;

    for (size_t x = 0; x < camelString.length() - 1; x++) {
        if (camelString[x] == '_') {
//            tempString = camelString.substr(x + 1, 1);
//            transform(tempString.begin(), tempString.end(), tempString.begin(), toupper);
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

        /* Make the replacement. */
        str.replace(index, from.size(), to);
        index += from.size();
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