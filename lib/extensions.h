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

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h>
#include <string.h>

using std::cout; 
using std::endl;

#define PUSH_EXTENSION(seg, func) std::pair<std::string, void(*)(void)>(seg, func)

extern std::vector<Exercise> Exercises;
extern Exercise* current_exercise;
extern std::map<std::string, std::string> Variables;
extern HANDLE hConsole;

extern std::string FetchValue (std::string);

/* POST START SCREEN EXTENSIONS */

void calculate_total_session_time () {
    // This extension calculates the total amount of time it takes to continue an exercise session
    FOREGROUND_COLOR(13);
    double total_time = 0.00;
    for (auto& exer : Exercises) {
        total_time += exer.CalculateTime();
    }
    total_time *= atoi(FetchValue("ROUNDS").c_str());
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
    total_reps *= atoi(FetchValue("ROUNDS").c_str());
    cout << "R: " << total_reps <<  endl;
    RESET_COLORS();
}

void n_exercises () {
    FOREGROUND_COLOR(8);
    cout << "N: " << Exercises.size() << endl;
    RESET_COLORS();
}

void display_info () {
    Log("Hello from display_info", 6);
}

/* POST EXERCISE EXTENSIONS */

void display_exercise_image () {
    // This extension displays a visual representation of the current exercise at hand
    if (atoi(FetchValue("DISPLAY").c_str())) {
        std::string png (current_exercise->name);
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        system(cmd.c_str());
    }
}


std::vector<std::pair<std::string, void(*)(void)>> Extensions = {
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_time),
    PUSH_EXTENSION("post_start_screen", &calculate_total_session_reps),
    PUSH_EXTENSION("post_start_screen", &n_exercises),
    PUSH_EXTENSION("post_exercise", &display_exercise_image)
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
