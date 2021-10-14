/* STD INCLUDES */
#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <utility>

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
extern std::vector<std::pair<std::string, std::string>> global_changeables;
extern bool is_dig (char*);

extern int _rounds;
extern int _start;
extern char _category;
extern bool _display;
extern bool _focus;
extern bool _reverse;
extern bool _bmode;
extern int _rdelay;
extern bool _dummy;
extern bool _use_start;
extern int _timemode;
extern int _del;
extern int setsoffset;
extern int repsoffset;
extern int holdoffset;
extern int aholdoffset;
extern bool easy;
extern bool warmup;
extern bool setall;
extern int sets;
extern int reps;
extern int hold;
extern int ahold;

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

std::vector<std::pair<std::string, std::string>>::iterator
fetch_variable (std::string name) {
    auto v = std::find_if(Mem.begin(), Mem.end(), [&](Variable v) {return v.identifier == name;});
    int vi = v - Mem.begin();
    global_changeables.push_back(std::pair<std::string, std::string>(std::find_if(Mem.begin(), Mem.end(), [&](Variable v) {return v.identifier == name;})->identifier, std::find_if(Mem.begin(), Mem.end(), [&](Variable v) {return v.identifier == name;})->value));
    return global_changeables.begin() + vi;
}

std::string get_variable (std::string name) {
    for (auto& p : global_changeables) {
        if (p.first == name) return p.second;
    }
}

void setv (std::string iden, int value) {
    for (auto& p : global_changeables) {
        if (p.first == iden) p.second = value;
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

void compile_variables () {
    _rounds = WRAP_CONVERT(FETCH_VARIABLE("ROUNDS"));
    _use_start = WRAP_CONVERT(FETCH_VARIABLE("USESTART"));
    if (_use_start) {
        _start = WRAP_CONVERT(FETCH_VARIABLE("START"));
    }
    _category = FETCH_VARIABLE("CATEGORY")->value[0];
    _display = WRAP_CONVERT(FETCH_VARIABLE("DISPLAY"));
    _focus = WRAP_CONVERT(FETCH_VARIABLE("FOCUS"));
    _reverse = WRAP_CONVERT(FETCH_VARIABLE("REVERSE"));
    _bmode = WRAP_CONVERT(FETCH_VARIABLE("BMODE"));
    _rdelay = WRAP_CONVERT(FETCH_VARIABLE("RDELAY"));
    _dummy = WRAP_CONVERT(FETCH_VARIABLE("DUMMY"));
    _timemode = WRAP_CONVERT(FETCH_VARIABLE("TMODE"));
    _del = WRAP_CONVERT(FETCH_VARIABLE("NEXTDELAY"));
    
    setsoffset = WRAP_CONVERT(FETCH_VARIABLE("SETSOFFSET"));
    repsoffset =  WRAP_CONVERT(FETCH_VARIABLE("REPSOFFSET"));
    holdoffset = WRAP_CONVERT(FETCH_VARIABLE("HOLDOFFSET"));
    aholdoffset = WRAP_CONVERT(FETCH_VARIABLE("AHOLDOFFSET"));
    easy = WRAP_CONVERT(FETCH_VARIABLE("EASY"));
    warmup = WRAP_CONVERT(FETCH_VARIABLE("WARMUP"));
    setall = WRAP_CONVERT(FETCH_VARIABLE("SETALL"));
    sets = WRAP_CONVERT(FETCH_VARIABLE("SETS"));
    reps = WRAP_CONVERT(FETCH_VARIABLE("REPS"));
    hold = WRAP_CONVERT(FETCH_VARIABLE("HOLD"));
    ahold = WRAP_CONVERT(FETCH_VARIABLE("AHOLD"));
}
