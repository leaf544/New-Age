#ifndef MACROS_H
#define MACROS_H

#include "data.h"

/* GENERAL USE MACROS */

#define STANDARD_EXIT_PROCEDURE() { \
    std::cin.get(); \
    std::exit(1);   \
}

#define ON_KEY_CLS() { \
    if (getch()) {      \
        system("cls");   \
    }                     \
}

#define FLOOP(t, it, n) for(t it = 0; it < n; it++)
#define FLOOPS(t, it, v,  n) for(t it = v; it < n; it++)

#define JUSTIFIED(ternary) ((ternary) == true)

#define CSTRING_EQUAL 0
#define IGNORE 1400
#define MS 1000

/* NEW AGE RELATED MACROS */

#define STARTUP_SETUP() system("title New Age");
#define FOREGROUND_COLOR(c) SetConsoleTextAttribute(hConsole, c);
#define BACKGROUND_COLOR(c) SetConsoleTextAttribute(hConsole, c);
#define RESET_COLORS() FOREGROUND_COLOR(DEFAULT_FOREGROUND);
#define WRAP_CONVERT(o) std::atoi(o->value.c_str());
#define FETCH_VARIABLE(name) std::find_if(Mem.begin(), Mem.end(), [](Variable v) {return v.identifier == name;})

#endif
