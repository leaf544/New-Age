/* STD INCLUDES */
#include <iostream>
#include <Windows.h>
#include <functional>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <conio.h>

/* INCLUDES */
#include "lib/exercise.hpp"
#include "lib/structs.hpp"
#include "lib/macros.h"
#include "lib/data.h"
#include "lib/extensions.h"

/* HIGH STATUS VARIABLES */
std::vector<Exercise> Exercises;
std::vector<Exercise>::iterator current_exercise;
std::vector<Variable> Mem;
std::FILE* file = std::fopen(FILE_PATH, "r");
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

/* GENERAL FUNCTIONS */
extern void craft_message (char*, int, ...);
extern void espeak (std::string, char speak = 'F');
extern void gather_variables (void);
extern void display_all_variables (void);

bool is_dig (char);
void time_out (void);
void bar (const char* label, int a, int b);

using std::cout;
using std::endl;

int main (void) {

    /* SETUP  */
    
    STARTUP_SETUP();

    if (file == nullptr) {
        char message[1028];
        craft_message(message, 1, FILE_PATH);
        std::perror(message);
        STANDARD_EXIT_PROCEDURE();
    }

    gather_variables();
    
    int _rounds = std::atoi(FETCH_VARIABLE("ROUNDS"));
    char _category = FETCH_VARIABLE("CATEGORY")[0];
    int _start = std::atoi(FETCH_VARIABLE("START"));
    bool _display = std::atoi(FETCH_VARIABLE("DISPLAY"));
    bool _focus = std::atoi(FETCH_VARIABLE("FOCUS"));

    std::fseek(file, 0, SEEK_SET);
    
    /* EXERCISE COMPILATION PHASE */
    
    char line [256];
    std::string s_line (line);
    bool ignore = false;
    bool suicide = false;
    
    while (std::fgets(line, 256, file) != NULL) {
        line[std::strlen(line) - 1] = ' ';
        s_line = line;

        if (suicide) break;
        
        switch (line[0]) {
        case CATEGORY_MARKER: {
            if (line[1] != _category) {
                ignore = true;
            } else {
                ignore = false;
            }
        } break;
        case EXERCISE_MARKER: {
            static int n_iterations = -1;
            
            if (ignore != true ) {
                n_iterations++;
                std::string e_name = s_line.substr(1, s_line.find("  ") - 1);
                auto pos = std::find_if(s_line.begin(), s_line.end(), is_dig);
                std::vector<int> values;

                if (pos == s_line.end()) {
                    Exercises.push_back(Exercise(e_name));
                } else {
                    int pos_index = pos - s_line.begin();
                    char workable_copy[50];
                    std::strcpy(workable_copy, s_line.substr(pos_index, s_line.length() - pos_index).c_str());
            
                    for (char* token = std::strtok(workable_copy, " "); token != NULL; token = std::strtok(NULL, " ")) {
                        if (!std::isalpha(token[0])) {
                            values.push_back(std::atoi(token));
                        } else {
                            values.push_back((int) token[0]);
                        }
                    }
                    
                    if (n_iterations == _start || n_iterations > _start) {
                        Exercises.push_back(Exercise(e_name, (char) values[1], values[0], values[2], values[3], values[4]));
                        if (_focus) {
                            suicide = true;
                        }
                    }
                    
                }
            } else {
                continue;
            }
            
        } break;
        default:
            continue;
            break;
        }
    }

    compile_extensions("after_compilation");
    
    FOREGROUND_COLOR(10);
    cout << "Initialization finished, press any key to start" << endl << endl;
    FOREGROUND_COLOR(DEFAULT_FOREGROUND);
    cout << "# First Exercise " << endl << endl;
    Exercises.front().Describe(); // Describe first exercises
    cout << "Number of exercises you'll be performing in this session: " << Exercises.size() << endl << endl;
    
    // Outer extensions

    compile_extensions("outer_extensions");
    
    if (getch()) {
        FOREGROUND_COLOR(2);
        cout << "Delay (in seconds): ";
        int delay = 0;
        std::cin >> delay;
        Sleep(delay * MS);
    }

    system("cls");
    RESET_COLORS();

        
    /* MAIN LOOP */
    FLOOP (int, r, _rounds) {
        //i = 0, next = i + 1 (1) /// next = (i == Exercises.size() - 1)
        for (int i = 0, next = i; i < Exercises.size(); i++, next = i == Exercises.size() - 1 ? next = 0 : next = i + 1){
            current_exercise = Exercises.begin() + i;
            espeak(current_exercise->name);

            compile_extensions("exercise_begin");
            
            int current_reps = 0;
            bool alternate = false;
            FLOOP (int, s, current_exercise->sets) {
                /* Sets Loop */
                Sleep(1 * MS);
                FLOOP (int, r, current_exercise->reps * 2) {
                    /* Rounds Loop */
                    if (!alternate) {
                        current_reps++;
                        bar("REPS: ", current_reps, current_exercise->reps);
                        espeak(std::to_string(current_reps), current_exercise->freestyle);
                        Sleep(current_exercise->hold * MS);
                    } else {
                        espeak("Alternate");
                        Sleep(current_exercise->ahold * MS);
                    }
                    alternate = (!alternate);
                }
                cout << "next value: " << next << endl;
                current_reps = 0;
                system("cls");
                RESET_COLORS();
                espeak(std::string(current_exercise->name).append("Finished"));
                espeak("Set " + std::to_string(s + 1) + " of " + std::to_string(current_exercise->sets) + "Finished");
                espeak("Take a break");
                if (next == 0 && i > 0)  {
                    espeak("Round " + std::to_string(r + 1) + " of " + std::to_string(_rounds) + " Finished");
                }
                cout << "# Next Exercise" << endl << endl;
                Exercises[next].Describe();
                time_out();
            }
        }
    }
    

    espeak("Excellent job!");
    
    std::fclose(file);
    
    std::cin.get();
    return 0;
}

bool is_dig (char c) {
    return (std::isdigit(c) == true);
}

void time_out() {
    FOREGROUND_COLOR(5);
    cout << "Press any key to continue exercise session" << endl << endl;
    ON_KEY_CLS();
}

void bar(const char* label, int a, int b) {
    FOREGROUND_COLOR(9);
    system("cls");
    std::string sbar(b, ' ');
    FLOOP(int, i, a) {
        sbar[i] = '#';
    }
    sbar.append(" " + std::to_string(a) + "/" + std::to_string(b));
    cout << label << sbar << endl;
}

//g++ core.cc general.cc lib/extensions.h -o core.exe && start core.exe && cls
