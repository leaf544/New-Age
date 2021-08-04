#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <iostream>
#include <string>

struct Variable {
    std::string identifier;
    std::string value;
    Variable() {}
    Variable (std::string i, std::string v) {
        this->identifier = i;
        this->value = v;
    }
};

#endif
