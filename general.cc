/* STD INCLUDES */
#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

using std::cout;
using std::endl;

/* C INCLUDES */
#include <stdarg.h>

/* INCLUDES */
#include "lib/hash_id.h"
#include "lib/structs.hpp"
#include "lib/macros.h"
#include "lib/data.h"

/* DECLARATIONS */
Variable control_variable;
extern std::FILE* file;
extern std::vector<Variable> Mem;
extern bool is_dig (char*);

void craft_message (char* message, int num, ...) {
    va_list tuple;
    va_start(tuple, num);
    for (int i = 0; i < num; i++) {
        const char* arg = va_arg(tuple, const char*);
        std::strcat(message, arg);
    }
    va_end(tuple);
}


void execute_after_n_ticks (void(*funcptr)(char*), int nticks, char* str) {
    static void(*ptr)(char*);
    static int left = 0;
    if(funcptr != nullptr && nticks != IGNORE) {
        ptr = funcptr;
        left = nticks;
    }
    else if(funcptr == nullptr && nticks == IGNORE) {
        left--;
        if(!left) {
            ptr(str);
            // reset for next use case
            ptr = nullptr;
        }
    }
}

int what_is (const char* str, int mode) {
    if (std::strcmp(str, "#INT") == CSTRING_EQUAL) {
        return 1;
    }
    if (std::strcmp(str, "#BOOL") == CSTRING_EQUAL) {
        return 2;
    }
    switch (mode) {
    case 1:
        FLOOP (int, i, std::strlen(str)) {
            if (std::isalpha(str[i]) != 1) return false;
        }
        return 10;
        break;
    }
    return -1;
}

void get_variable_name (char* str) {
    control_variable.identifier = str;
}

void get_variable_value (char* str) {
    control_variable.value = str;
    Mem.push_back(control_variable);
}

void gather_variables () {
    char line [256];
    std::string s_line (line);
    while (std::fgets(line, 256, file) != NULL) {
        line[std::strlen(line) - 1] = ' ';
        switch (line[0]) {
        case VARIABLE_MARKER: {
            int i = 0;
            void(*fptr)(char*) = get_variable_name;
            for (char* token = std::strtok(line, " "); token != NULL; token = std::strtok(NULL, " ")) {
                execute_after_n_ticks(nullptr, IGNORE, token);
                // Type checks
                if (std::strcmp(token, "#INT") == CSTRING_EQUAL) {
                    execute_after_n_ticks(get_variable_name, 1, token);
                }
                if (std::strcmp(token, "#BOOL") == CSTRING_EQUAL) {
                    execute_after_n_ticks(get_variable_name, 1, token);
                }
                if (std::strcmp(token, "#CHAR") == CSTRING_EQUAL) {
                    execute_after_n_ticks(get_variable_name, 1, token);
                }
                // Other important checks
                if (what_is(token, 1) == 10) {
                    execute_after_n_ticks(get_variable_value, 1, token);
                }
            }
            break;
        }
        default:
            continue;
            break;
        }
    }
}

void espeak (std::string message, char free) {
    if (free == 'F') {
        std::replace(message.begin(), message.end(), ' ', '_');
        char system_command [128] = "espeak ";
        std::strcat(system_command, message.c_str());
        std::system(system_command);
    }
}

void display_all_variables () {
    for (auto& val : Mem) {
        cout << val.identifier << ": " << val.value << endl;
    }
}
