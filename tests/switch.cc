#include "all.h"

int main(void) {

    std::FILE* file = std::fopen("text.txt", "r");
    char line[256];
    while (std::fgets(line, 256, file) != NULL) {
        line[std::strlen(line)-1] = ' ';
        char head = line[0];
        switch (head) {
        case '$': {
            static int num = 1;
            num++;
            cout << num << endl;
        } break;
        default:
            continue;
            break;
        }
    } 
    
    std::fclose(file);

    std::cin.get();
    return 0;
}
