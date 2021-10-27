#include <iostream>
#include <sstream>
#include <vector>
#include <string>

std::vector<std::string> split_string (std::string str, char delim) {
    std::vector<std::string> ret;
    std::istringstream split(str);
    for (std::string each; std::getline(split, each, delim); ret.push_back(each));
    return ret;
}
