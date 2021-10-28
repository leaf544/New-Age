/* STD */
#include <iostream>
#include <vector>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <sstream>
#include <fstream>

/* LIB */
#include "lib/macros.h"
#include "lib/data.h"
#include "lib/utilities.h"
#include "lib/exercise.hpp"
#include "lib/extensions.h"

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h>
#include <string.h>

std::vector<Exercise> Exercises;
Exercise* current_exercise = NULL;
std::map<std::string, std::string> Variables;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

std::string FetchValue (std::string);
void bar (const char* label, int a, int b);

using std::cout;
using std::endl;
using std::cin;

class Reader {
public:
    int i = 0;
    int limit = 0;
    std::vector<Exercise>* list;
    void attatch (std::vector<Exercise>* l) {this->list = l; this->limit = l->size();}
    Exercise* at (int pos) {return &list->at(pos);}
    void set (int pos) {this->i = pos;}
    void progress () {this->i++;}
    void regress () {this->i--;}
};


int main (void) {
    
    /* SETUP & PREREQUISITES */
    
    STARTUP_SETUP();    
    
    /* COMPILE EXERCISES AND VARIABLES */

    std::ifstream file(FILE_PATH);
    std::string category = "";
    
    for (std::string line; std::getline(file, line);) {
        if (line[0] == VARIABLE_MARKER) {
            auto values = UTIL::split_string(line.substr(1, line.length()), " ");
            Variables[values[1]] = values[2];
        }
        if (line[0] == '%') {
            category = line.substr(1, line.length());
        }
        if (line[0] == EXERCISE_MARKER and category == Variables["CATEGORY"]) {
            auto values = UTIL::split_string(line.substr(1, line.length()), " ");
            Exercise instance (values[0]);
            instance.sets = atoi(values[1].c_str());
            instance.freestyle = values[2][0];
            instance.reps = atoi(values[3].c_str());
            instance.hold = atoi(values[4].c_str());
            instance.ahold = atoi(values[5].c_str());
            Exercises.push_back(instance);
        }
    }

    /* POST COMPILATION */

    compile_extensions("post_compilation");
    
    /* START SCREEN */

    compile_extensions("post_start_screen");
    RESET_COLORS();
    //cout << endl;
    cout << Exercises.front().name << ", " << Exercises.front().sets << ", " << Exercises.front().freestyle << ", " << Exercises.front().reps << ", " << Exercises.front().hold << ", " << Exercises.front().ahold << endl;
    ON_KEY_CLS();
    
    /* MAIN LOOP */
    
    Reader reader;
    reader.attatch(&Exercises);
    
    bool finished = false;
    
    FLOOP (int, ROUNDS, atoi(FetchValue("ROUNDS").c_str())) {
        current_exercise = reader.at(atoi(FetchValue("START").c_str()));
        while (not finished) {

            /* Begin Exercise Block */
            UTIL::espeak(current_exercise->name);
            compile_extensions("post_exercise");
            /* End Exercise Block */
            
            FLOOP (int, SETS, current_exercise->sets) {
                /* Begin Sets Block */
                bool alternate = false;
                bool skipped = false;
                int  current_reps = 0;
                Sleep(atoi(FetchValue("RDELAY").c_str()) * MS);
                /* End Sets Block */
                FLOOP (int, REPS, current_exercise->reps * 2) {
                    if (not alternate) {
                        bar("REPS: ", current_reps, current_exercise->reps);
                        current_reps++;
                        UTIL::espeak(std::to_string(current_reps), current_exercise->freestyle);
                        SLEEP_TIME_FUNCTION(current_exercise->hold, if (GetAsyncKeyState(VK_RIGHT)){
                                Log("Skipping..", 4);
                                skipped = true;
                                break;
                            });
                        
                        if (skipped) break;
                    } else {
                        UTIL::espeak("Alternate", current_exercise->freestyle);
                        bar("REPS: ", current_reps, current_exercise->reps);
                        SLEEP_TIME_FUNCTION(current_exercise->ahold, if (GetAsyncKeyState(VK_RIGHT)){
                                Log("Skipping..", 4);
                                skipped = true;
                                break;
                            });
                        
                        if (skipped) break;
                    }
                    alternate = (!alternate);
                }
                UTIL::espeak("Set " + std::to_string(SETS + 1) + " of " + std::to_string(current_exercise->sets) + " Finished");
            }

            CLEAR();
            UTIL::espeak(std::string(current_exercise->name).append(" Finished"));
            UTIL::espeak("Take a break");

            if (reader.i == reader.limit - 1) {
                finished = true;
            } else {
                reader.progress();
                current_exercise = reader.at(reader.i);
                cout << "# Next Exercise" << endl;
                current_exercise->Describe();
                ON_KEY_CLS();
            }
        }
    }

    UTIL::espeak("Enjoy the feast!");
    
    cin.get();
    return 0;
}

std::string FetchValue (std::string iden) {
     return Variables[iden] != "" ? Variables[iden] : "0";
}

void bar(const char* label, int a, int b) {
    FOREGROUND_COLOR(11);
    CLEAR();
    std::string sbar(b, ' ');
    FLOOP(int, i, a) {
        sbar[i] = '#';
    }
    sbar.append(" " + std::to_string(a) + "/" + std::to_string(b));
    cout << label << sbar << endl;
    RESET_COLORS();
}
