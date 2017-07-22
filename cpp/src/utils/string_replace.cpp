//
// Created by sabir on 22.07.17.
//

string string_replace(string str, string from, string to){
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