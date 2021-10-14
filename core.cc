/* STD INCLUDES */
#include <iostream>
#include <Windows.h>
#include <functional>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <csignal>
#include <string>
#include <vector>
#include <utility>

/* C INCLUDES */

#include <conio.h>
#include <time.h>

/* INCLUDES */
#include "lib/exercise.hpp"
#include "lib/structs.hpp"
#include "lib/macros.h"
#include "lib/data.h"
#include "lib/extensions.h"

#define MAX_HOLD 100

/* HIGH STATUS VARIABLES */
std::vector<Exercise> Exercises;
std::vector<Exercise>::iterator current_exercise;
std::vector<Variable> Mem;
std::vector<std::pair<std::string, std::string>> global_changeables;
std::FILE* file = std::fopen(FILE_PATH, "r");
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

/* GENERAL FUNCTIONS */
extern void craft_message (char*, int, ...);
extern void espeak (std::string, char speak = 'F');
extern void gather_variables (void);
extern std::vector<std::pair<std::string, std::string>>::iterator fetch_variable (std::string);
extern void display_all_variables (void);
extern void compile_variables (void);

bool is_dig (char);
void time_out (int);
void bar (const char* label, int a, int b);
    
int _rounds = 0;
int _start = IGNORE;
char _category;
bool _display = false;
bool _focus = false;
bool _reverse = false;
bool _bmode = false;
bool _dummy = false;
int _timemode = 0;
bool _use_start = false;
int _del = 0;
int _rdelay = 0;
int setsoffset = 0;
int repsoffset = 0;
int holdoffset = 0;
int aholdoffset = 0;
bool easy = false;
bool warmup = false;
bool setall = false;
int sets = 0;
int reps = 0;
int hold = 0;
int ahold = 0;

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
    compile_variables();
    
    std::fseek(file, 0, SEEK_SET);
    
    /* EXERCISE COMPILATION PHASE */
    
    char line [256];
    std::string s_line (line);
    bool ignore = false;
    bool suicide = false;
    bool found_start_vary = false;
    
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
                
                if (line[1] == START_MARKER) {
                    _start = n_iterations;
                    found_start_vary = true;
                }
                // std::string e_name = s_line.substr(1, s_line.find("  ") - 1);
                std::string e_name = s_line.substr(found_start_vary == true ? 2 : 1 , s_line.find("  ") - (found_start_vary == true ? 2 : 1));
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
                        Exercises.push_back(Exercise(e_name, (char) values[1], values[0] + setsoffset, values[2] + repsoffset, values[3] + holdoffset, values[4] + aholdoffset));
                        Exercises.back().tags = s_line.find(NO_DISPLAY_MARKER) != std::string::npos ? "no_display" : "";
                        //Exercises.back().tags = s_line.find(CONTROL_MARKER) != std::string::npos ? "control" : "";
                        if (_focus) {
                            suicide = true;
                        }
                    }
                    
                }
                found_start_vary = false;
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
    cout << "Initialization finished, press any key to start" << endl;
    FOREGROUND_COLOR(8);
    cout << "First Exercise display may not be accurate if REVERSE mode is activated" << endl << endl;
    RESET_COLORS();
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
        if (!std::atoi(FETCH_VARIABLE("REVERSE")->value.c_str())) {
            _start = 0;
        }
        
        for (int i = 0 + _start, next = i + 1; i < Exercises.size(); i++, next = (i == Exercises.size() - 1 ? next = 0 : next = i + 1)){
            current_exercise = Exercises.begin() + i;
            espeak(current_exercise->name);
            
            compile_extensions("exercise_begin");
            
            int current_reps = 0;
            bool alternate = false;
            Sleep(_rdelay * MS);
            FLOOP (int, s, current_exercise->sets) {
                /* Sets Loop */
                Sleep(1 * MS);
                bool skip = false;
                int multip = current_exercise->tags == "control" ? 1 : 2;
                FLOOP (int, r, current_exercise->reps * multip) {
                    if (current_exercise->tags == "control") {
                        long long start = time(NULL);
                        long long elapsed = 0;
                        long long last = 0;
                        bool block = false;
                        current_reps++;
                        
                        // while(elapsed < MAX_HOLD) {
                        //     if (elapsed >= current_exercise->hold) {
                        //         if (!block) {
                        //             bar("SECONDS: ", elapsed, current_exercise->hold);
                        //             espeak("Hold exceeded");
                        //             current_reps++;
                        //             block = true;
                        //         }
                        //         if (GetAsyncKeyState(VK_UP)) {
                        //             block = false;
                        //             break;
                        //         }
                        //     }
                        //     if (elapsed != last) {                    
                        //         bar("SECONDS: ", elapsed, current_exercise->hold);
                        //         if (block) {
                        //             cout << elapsed - current_exercise->hold << " extra seconds" << endl;                                   
                        //         }
                        //     }
                        //     last = elapsed;
                        //     elapsed = time(NULL) - start;
                        // }
                        std::system("cls");
                        Sleep(current_exercise->ahold * MS);
                    } else {
                        if (!alternate) {
                            current_reps++;
                            bar("REPS: ", current_reps, current_exercise->reps);
                            // espeak(std::to_string(current_reps), current_exercise->freestyle);
                            espeak(std::to_string(current_reps), current_exercise->freestyle);
                            Sleep(current_exercise->hold * MS);
                        } else {
                            espeak("Alternate", current_exercise->freestyle);
                             Sleep(current_exercise->ahold * MS);
                        
                        //     long long start = time(NULL);
                        //     long long elapsed = 0;

                        //     while(elapsed < current_exercise->ahold) {
                        //         if (GetAsyncKeyState(VK_UP)) {
                        //             skip= true;
                        //             cout << "Skipping.." << endl;
                        //             Sleep(1 * MS);
                        //             RESET_COLORS();
                        //             system("cls");
                        //             break;
                        //         }
                        //         elapsed = time(NULL) - start;
                        //     }
                        
                        // }
                        }
                        alternate = (!alternate);
                        if (skip) break;
                    }

                }
                if (skip) {
                    skip = false;
                    break;
                }
                // cout << "next value: " << next << endl;
                // cout << "justified?: " << JUSTIFIED(next == (i == Exercises.size() - 1)) << endl;
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
                time_out(_timemode);
            }
        }
        // compile_extensions("round_end");
    }
    

    espeak("Excellent job!");
    
    std::fclose(file);
    
    std::cin.get();
    return 0;
}

bool is_dig (char c) {
    return (std::isdigit(c) == true);
}

void time_out(int m) {
    if (m) {
        Sleep(_del * MS);
        std::system("cls");
    } else {
        FOREGROUND_COLOR(5);
        cout << "Press any key to continue exercise session" << endl << endl;
        ON_KEY_CLS();    
    }
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


// g++ core.cc general.cc lib/extensions.h -o core.exe && start core.exe && cls
// id like my chinese food cooked right, cause they're not giving me the good rice and the dyke giving me my food looks like a resbian
