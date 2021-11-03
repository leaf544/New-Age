/* Written by leaf544 aka grandma */

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
#include "lib/category.hpp"

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h>
#include <string.h>

std::vector<Exercise> Exercises;
Exercise* current_exercise = NULL;
Category living_category;
std::map<std::string, std::string> Variables;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

std::string FetchValue (std::string);
int FetchValueInt (std::string);
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
    std::vector<std::string> values;

    /* PRE COMPILATION */
    
    for (std::string line; std::getline(file, line);) {
        if (line[0] == VARIABLE_MARKER) {
            values = UTIL::split_string(line.substr(1, line.length()), " ");
            Variables[values[1]] = values[2];
            values.clear();
        }
        if (line[0] == '%') {
            /* Category Block Begin */
            values = UTIL::split_string(line.substr(1, line.length()), " ");
            category = values[0];

            if (category == FetchValue("CATEGORY") and values.size() > 1) {
                living_category.hasVariables = true; // IMPORTANT
                values.erase(values.begin());
                auto tmp = values;
                int p = 1;
                FLOOP (int, n, tmp.size() / 3) {
                    living_category.Variables[tmp[p]] = tmp[p+1];
                    p += 3;
                }
            }
            /* Category Block End */
        }
        if (line[0] == EXERCISE_MARKER and category == Variables["CATEGORY"]) {
            values = UTIL::split_string(line.substr(1, line.length()), " ");
            Exercises.push_back(Exercise(values[0], values[2][0], CATOI(values[1]), CATOI(values[3]), CATOI(values[4]), CATOI(values[5])));
            if (values.size() > 6) {
                //Exercises.back().tags = values
                FLOOPS (int, i, 6, values.size()) {
                    Exercises.back().tags.append(values[i] + " ");
                }
            }
            values.clear();
        }
    }

    // living_category.ListVariables();
    
    if (Exercises.size() == 0) {
        Log("No exercises were registered", 8);
        ON_KEY_CLS();
        return 0;
    }
    
    /* POST COMPILATION */

    compile_extensions("post_compilation");
    
    /* START SCREEN */
    RESET_COLORS();
    compile_extensions("post_start_screen");
    RESET_COLORS();
    cout << endl;
    FOREGROUND_COLOR(240);
    (Exercises.begin())->Describe2();
    RESET_COLORS();
    ON_KEY_CLS();
    
    /* MAIN LOOP */
    
    Reader reader;
    reader.attatch(&Exercises);
    
    bool finished = false;
    
    FLOOP (int, ROUNDS, (DETERMINE_VALUE("ROUNDS", FetchValueInt))) {
        reader.set(0);
        current_exercise = reader.at(0);
        reader.attatch(&Exercises);
        
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
                Sleep((DETERMINE_VALUE("RDELAY", FetchValueInt)) * MS);
                /* End Sets Block */
                FLOOP (int, REPS, current_exercise->reps * 2) {
                    if (not alternate) {
                        bar("REPS: ", current_reps, current_exercise->reps);
                        current_reps++;
                        UTIL::espeak(std::to_string(current_reps), current_exercise->freestyle);
                        SLEEP_TIME_FUNCTION(current_exercise->hold, if (GetAsyncKeyState(0x39)){
                                Log("Skipping..", 4);
                                skipped = true;
                                break;
                            });
                        
                        if (skipped) break;
                    } else {
                        UTIL::espeak("Alternate", current_exercise->freestyle);
                        bar("REPS: ", current_reps, current_exercise->reps);
                        SLEEP_TIME_FUNCTION(current_exercise->ahold, if (GetAsyncKeyState(0x39)){
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
        /* Round End Block */
        finished = false;
        compile_extensions("round_end");
    }
    
    UTIL::espeak("Enjoy the feast!");
    
    cin.get();
    return 0;
}

std::string FetchValue (std::string iden) {
    return Variables[iden] != "" ? Variables[iden] : "0";
}

int FetchValueInt (std::string iden) {
    return atoi(FetchValue(iden).c_str());
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

