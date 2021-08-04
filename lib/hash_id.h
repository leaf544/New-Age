#ifndef HASH_H
#define HASH_H

#include <cstring>

enum HASH_ID {
    HASH_INT = 3,
    HASH_BOOL = 4
};

int hash (const char* str) {
    return ((HASH_ID) std::strlen(str) - 1);
}

#endif 
