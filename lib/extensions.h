#ifndef EXTENSIONS_H
#define EXTENSIONS_H

/* STD */
#include <iostream>
#include <vector>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <utility>
#include <iomanip>

/* LIB */
#include "macros.h"
#include "data.h"
#include "utilities.h"
#include "exercise.hpp"
#include "category.hpp"

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <string.h>

using std::cout; 
using std::endl;

#define PUSH_EXTENSION(seg, func) std::pair<std::string, void(*)(void)>(seg, func)

extern std::vector<Exercise> Exercises;
extern Exercise* current_exercise;
extern std::map<std::string, std::string> Variables;
extern Category living_category;
extern HANDLE hConsole;

extern std::string FetchValue (std::string);
extern int FetchValueInt (std::string);

/* POST COMPILATION EXTENSIONS */

void reverse_exercises () {
    if (DETERMINE_VALUE("REVERSE", FetchValueInt)) {
        std::reverse(Exercises.begin(), Exercises.end());
    }
}

void handle_start () {
    Exercises.erase(Exercises.begin(), Exercises.begin() + (DETERMINE_VALUE("START", FetchValueInt)));
}

void on_mood () {
    if (DETERMINE_VALUE("LOW_MOOD", FetchValueInt)) {
        for (auto& exer : Exercises) {
            exer.reps = 12;
        }
    }
}

void handle_offsets () {
    for (auto& exer : Exercises) {
        exer.sets += (DETERMINE_VALUE("SETS_OFFSET", FetchValueInt));
        exer.reps += (DETERMINE_VALUE("REPS_OFFSET", FetchValueInt));
        exer.hold += (DETERMINE_VALUE("HOLD_OFFSET", FetchValueInt));
        exer.ahold += (DETERMINE_VALUE("AHOLD_OFFSET", FetchValueInt));
    }
}

/* POST START SCREEN EXTENSIONS */

void calculate_total_session_time () {
    // This extension calculates the total amount of time it takes to continue an exercise session
    FOREGROUND_COLOR(13);
    double total_time = 0.00;
    for (auto& exer : Exercises) {
        total_time += exer.CalculateTime();
    }
    total_time *= DETERMINE_VALUE("ROUNDS", FetchValueInt);
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    cout << "T: " << total_time << " minutes" << endl;
    RESET_COLORS();
}

void calculate_total_session_reps () {
    // This extension calculates the total amount of reps in a session
    FOREGROUND_COLOR(6);
    int total_reps = 0;
    for (auto& exer : Exercises) {
        total_reps += exer.reps * exer.sets;
    }
    total_reps *= (DETERMINE_VALUE("ROUNDS", FetchValueInt));
    cout << "R: " << total_reps <<  endl;
    RESET_COLORS();
}

void n_exercises () {
    FOREGROUND_COLOR(8);
    cout << "N: " << Exercises.size() << endl;
    RESET_COLORS();
}

void display_variables () {
    int colors = 0;
    RESET_COLORS(); // Just in case, evaluate later
    if (living_category.Variables.size()) {
        cout << endl;
        for (auto it = living_category.Variables.begin(); it != living_category.Variables.end(); it++) {
            FOREGROUND_COLOR(colors);
            if (it->second != "") {
                cout << it->first << ": " << it->second << endl;    
            }
            colors++;
        }
        RESET_COLORS(); // Just in case, evaluate later
    }
}

void display_info () {
    Log("Hello from display_info", 6);
}

/* POST EXERCISE EXTENSIONS */

void display_exercise_image () {
    // This extension displays a visual representation of the current exercise at hand
    if (FetchValueInt("DISPLAY") and not living_category.hasVariables and current_exercise->tags.find("NO_DISPLAY") == std::string::npos) {
        std::string png (current_exercise->name);
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
    } else if (living_category.hasVariables and living_category.FetchValueInt("DISPLAY") and current_exercise->tags.find("NO_DISPLAY") == std::string::npos) {
        std::string png (current_exercise->name);
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
    }
}

/* POST ROUND */

void multi_round_session () {
    if ((DETERMINE_VALUE("ROUNDS", FetchValueInt)) > 1 and current_exercise->tags.find("NO_NEG") == std::string::npos) {
        for (auto& exer : Exercises) {
            exer.reps -= (DETERMINE_VALUE("NEG", FetchValueInt));
        }
        std::reverse(Exercises.begin(), Exercises.end());
    }
}

std::vector<std::pair<std::string, void(*)(void)>> Extensions = {
    //// POST COMPILATION ////
    PUSH_EXTENSION("post_compilation", &reverse_exercises),
    PUSH_EXTENSION("post_compilation", &handle_start),
    PUSH_EXTENSION("post_compilation", &on_mood),
    PUSH_EXTENSION("post_compilation", &handle_offsets),
    //// POST START SCREEN ////
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_time),
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_reps),
    PUSH_EXTENSION("post_start_screen", &n_exercises),
    PUSH_EXTENSION("post_start_screen", &display_variables),
    //// POST EXERCISE ////
    PUSH_EXTENSION("post_exercise", &display_exercise_image),
    //// POST ROUND END ////
    PUSH_EXTENSION("round_end", &multi_round_session)
};

void compile_extensions (std::string group) {
    for (auto& p :Extensions) {
        if (p.first == group) {
            p.second();
            FOREGROUND_COLOR(3);
        }
    }
}


#endif
