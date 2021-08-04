#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <vector>
#include <utility>
#include <cstdlib>
#include <string>
#include <conio.h>

#include "structs.hpp"
#include "exercise.hpp"
#include "macros.h"
#include "utilities.h"

using std::cout;
using std::endl;


#define PUSH_EXTENSION(seg, func) std::pair<std::string, void(*)(void)>(seg, func)

extern std::vector<Variable> Mem;
extern std::vector<Exercise> Exercises;
extern std::vector<Exercise>::iterator current_exercise;
extern HANDLE hConsole;

/* EXTENSIONS */


/* AFTER COMPILATION EXTENSIONS */

void reverse_exercises () {
    // This extension reverses the order of exercises and also provides revese compatibility 
    bool reverse = std::atoi(FETCH_VARIABLE("REVERSE"));
    if (reverse) {
        std::reverse(Exercises.begin(), Exercises.end());
    }
}

void debug_tools () {
    cout << "# Debug" << endl;
}

/* OUTER EXTENSIONS */

void calculate_total_session_time () {
    // This extension calculates the total amount of time it takes to continue an exercise session
    FOREGROUND_COLOR(13);
    long long total_time = 0;
    for (auto& exer : Exercises) {
        total_time += exer.CalculateTime();
    }
    cout << "Total session time: " << total_time << " minutes" << endl << endl;
    RESET_COLORS();
}

void describe_all_exercises () {
    // This extension loops through all class objects of the Exercises vector and invokes their "Describe" method 
    for (auto& exer : Exercises) {
        exer.Describe();
    }
}


/* ROUNDS END EXTENSIONS */


/* ROUNDS BEGIN EXTENSIONS */


/* EXERCISE BEGIN EXTENSIONS */

void display_exercise_image (void) {
    // This extension displays a visual representation of the current exercise at hand
    bool display = std::atoi(FETCH_VARIABLE("DISPLAY"));
    if (display) {
        std::string png (current_exercise->name);
        FLOOP(int, fi, png.length()) {
            if(png[fi] == ' ') png[fi] = '_';
        }
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        std::system(cmd.c_str());
    }
}

/* EXERCISE END EXTENSIONS */

std::vector<std::pair<std::string, void(*)(void)>> Extensions = {
    PUSH_EXTENSION("exercise_begin", &display_exercise_image),
    PUSH_EXTENSION("outer_extensions", &calculate_total_session_time),
    PUSH_EXTENSION("after_compilation", &reverse_exercises),
    // PUSH_EXTENSION("outer_extensions", &describe_all_exercises)
};

void compile_extensions (std::string group) {
    for (auto& p : Extensions) {
        if (p.first == group) p.second();
    }
}

#endif
