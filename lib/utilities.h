/* STD */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

/* C */
#include <stdlib.h>
#include <string.h> // strtok(), strcat()


#ifndef UTIL_H
#define UTIL_H

namespace UTIL {
    
    std::vector<std::string> split_string (std::string str, char* delim) {
        std::vector<std::string> ret;
        char cstring [str.length() + 1];
        strcpy(cstring, str.c_str());
        for (char* token = strtok(cstring, delim); token != NULL; token = strtok(NULL, delim)) {
            ret.push_back(std::string(token));
        } 
        return ret;
    }
    
    void espeak (std::string message, char free = 'F') {
        if (free == 'F') {
            std::replace(message.begin(), message.end(), ' ', '_');
            char system_command [128] = "espeak ";
            strcat(system_command, message.c_str());
            system(system_command);
        }
    }
    
}


#endif
