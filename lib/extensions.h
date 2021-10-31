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
    Exercises.erase(Exercises.begin(), Exercises.begin() + FetchValueInt("START"));
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
    for (auto it = Variables.begin(); it != Variables.end(); it++) {
        if (it->second != "") {
            FOREGROUND_COLOR(colors);
            cout << it->first << ": " << it->second << endl;
            colors++;
        } 
    }
    RESET_COLORS(); // Just in case, evaluate later
}

void display_info () {
    Log("Hello from display_info", 6);
}

/* POST EXERCISE EXTENSIONS */

void display_exercise_image () {
    // This extension displays a visual representation of the current exercise at hand
    if (FetchValueInt("DISPLAY") and not living_category.hasVariables) {
        std::string png (current_exercise->name);
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
    } else if (living_category.hasVariables and living_category.FetchValueInt("DISPLAY")) {
        std::string png (current_exercise->name);
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
    }
}

/* POST ROUND */

void multi_round_session () {
    if ((DETERMINE_VALUE("ROUNDS", FetchValueInt)) > 1) {
        std::reverse(Exercises.begin(), Exercises.end());
    }
}

std::vector<std::pair<std::string, void(*)(void)>> Extensions = {
    PUSH_EXTENSION("post_compilation", &reverse_exercises),
    PUSH_EXTENSION("post_compilation", &handle_start),
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_time),
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_reps),
    PUSH_EXTENSION("post_start_screen", &n_exercises),
    PUSH_EXTENSION("post_start_screen", &display_variables),
    PUSH_EXTENSION("post_exercise", &display_exercise_image),
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
