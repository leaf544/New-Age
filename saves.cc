    
    // _rounds = std::atoi(FETCH_VARIABLE("ROUNDS")->value.c_str());
    // bool _use_start = std::atoi(FETCH_VARIABLE("USESTART")->value.c_str());
    // if (_use_start) {
    //     _start = std::atoi(FETCH_VARIABLE("START")->value.c_str());
    // }
    // _category = FETCH_VARIABLE("CATEGORY")->value[0];
    // _display = std::atoi(FETCH_VARIABLE("DISPLAY")->value.c_str());
    // _focus = std::atoi(FETCH_VARIABLE("FOCUS")->value.c_str());
    // _reverse = std::atoi(FETCH_VARIABLE("REVERSE")->value.c_str());
    // _bmode = std::atoi(FETCH_VARIABLE("BMODE")->value.c_str());
    // int _rdelay = std::atoi(FETCH_VARIABLE("RDELAY")->value.c_str());
    // _dummy = std::atoi(FETCH_VARIABLE("DUMMY")->value.c_str());
    // int _timemode = std::atoi(FETCH_VARIABLE("TMODE")->value.c_str());
    // _del = std::atoi(FETCH_VARIABLE("NEXTDELAY")->value.c_str());
    
    // int setsoffset = WRAP_CONVERT(FETCH_VARIABLE("SETSOFFSET"));
    // int repsoffset =  WRAP_CONVERT(FETCH_VARIABLE("REPSOFFSET"));
    // int holdoffset = WRAP_CONVERT(FETCH_VARIABLE("HOLDOFFSET"));
    // int aholdoffset = WRAP_CONVERT(FETCH_VARIABLE("AHOLDOFFSET"));
    
    // int setsoffset = std::atoi(FETCH_VARIABLE("SETSOFFSET")->value.c_str());
    // int repsoffset = std::atoi(FETCH_VARIABLE("REPSOFFSET")->value.c_str());
    // int holdoffset = std::atoi(FETCH_VARIABLE("HOLDOFFSET")->value.c_str());
    // int aholdoffset = std::atoi(FETCH_VARIABLE("AHOLDOFFSET")->value.c_str());
  for (auto& p : Extensions) {
    if (p.first == group) {p.second();
    }
  }


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

    /* START SCREEN */

    compile_extensions("post_start_screen");
    RESET_COLORS();
    cout << endl;
    Exercises.front().Describe();
    //cout << endl;
    ON_KEY_CLS();
    
    /* MAIN LOOP */
    
    Reader reader;
    reader.attatch(&Exercises);
    
    bool finished = false;
    current_exercise = reader.at(0);
    
    FLOOP (int, ROUNDS, atoi(FetchValue("ROUNDS").c_str())) {
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
                        SLEEP_TIME_FUNCTION(current_exercise->ahold, if (GetAsyncKeyState(VK_RIGHT)){
                                Log("Skipping..", 4);
                                skipped = true;
                                break;
                            });
                        
                        if (skipped) break;

                        //Sleep(current_exercise->hold * MS);
                    } else {
                        UTIL::espeak("Alternate");
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
    
    std::cin.get();
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
