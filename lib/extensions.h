#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <iostream>
#include <iomanip>
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

extern int _rounds;
extern int _start;
extern char _category;
extern bool _display;
extern bool _focus;
extern bool _reverse;
extern bool _bmode;
extern bool _dummy;
extern bool easy;
extern bool warmup;
extern bool setall;
extern int sets;
extern int reps;
extern int hold;
extern int ahold;

/* EXTENSIONS */


/* AFTER COMPILATION EXTENSIONS */

void reverse_exercises () {
    // This extension reverses the order of exercises and also provides revese compatibility 
    if (_reverse) {
        std::reverse(Exercises.begin(), Exercises.end());
    }
}

void on_bmode () {
    if (_bmode) {
        std::vector<Exercise> cpy = Exercises;
        std::reverse(cpy.begin(), cpy.end());
        Exercises.insert(Exercises.end(), cpy.begin(), cpy.end());
    }
}

void on_dummy () {
    if (_dummy) {
        for (auto& exer : Exercises) {
            exer.sets = 1;
            exer.reps = 1;
            exer.hold = 0;
            exer.ahold = 0;
        }
    }
}


void on_easy () {
    if (easy) {
        for (auto& exer : Exercises) {
          exer.reps = 6;
        }
    }
}

void on_warmup () {
    if (warmup) {
        for (auto& exer : Exercises) {
            exer.reps = 4;
        }
    }
}

void debug_tools () {
    cout << "# Debug" << endl;
}

void on_setall () {
    if (setall) {
        for (auto& exer : Exercises) {
            exer.sets = sets;
            exer.reps = reps;
            exer.hold = hold;
            exer.ahold = ahold;
        }
    }
}


/* OUTER EXTENSIONS */

void calculate_total_session_time () {
    // This extension calculates the total amount of time it takes to continue an exercise session
    FOREGROUND_COLOR(13);
    double total_time = 0.00;
    for (auto& exer : Exercises) {
        total_time += exer.CalculateTime();
    }
    total_time *= _rounds;
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    cout << "Total session time: " << total_time << " minutes" << endl << endl;
    RESET_COLORS();
}


void calculate_total_session_reps () {
    // This extension calculates the total amount of time it takes to continue an exercise session
    FOREGROUND_COLOR(13);
    long long total_reps = 0;
    for (auto& exer : Exercises) {
        total_reps += exer.reps * exer.sets;
    }
    total_reps *= _rounds;
    cout << "Total session reps: " << total_reps <<  endl << endl;
    RESET_COLORS();
}


void describe_all_exercises () {
    // This extension loops through all class objects of the Exercises vector and invokes their "Describe" method 
    for (auto& exer : Exercises) {
        exer.Describe();
    }
}

/* ROUNDS END EXTENSIONS */

void reverse_mode_activate () {
    std::vector<Exercise> temp;
    //std::copy(Exercises.begin(), Exercises.end(), std::back_inserter(temp));
    for (auto& val : Exercises) {
        temp.push_back(val);
    }
    std::reverse(Exercises.begin(), Exercises.end());
    for (auto& val : temp) {
        Exercises.push_back(val);
    }
}

/* ROUNDS BEGIN EXTENSIONS */


/* EXERCISE BEGIN EXTENSIONS */

void display_exercise_image (void) {
    // This extension displays a visual representation of the current exercise at hand
    if (_display && current_exercise->tags != "no_display") {
        std::string png (current_exercise->name);
        FLOOP(int, fi, png.length()) {
            if(png[fi] == ' ') png[fi] = '_';
        }
        png.append(".png");
        std::string cmd = "start " + std::string(FILE_PNGS_PATH) + std::string(png) + " && timeout 4 && taskkill /IM Microsoft.Photos.exe /F";
        std::system(cmd.c_str());
    }
}

void display_end_results (void) {
    cout << "# End results" << endl;
    int total_reps = 0;
    for (auto& e : Exercises) {
        total_reps += e.reps;
    }
    total_reps *= _rounds;
    cout << "Total reps: " << total_reps << endl;
}

/* EXERCISE END EXTENSIONS */

std::vector<std::pair<std::string, void(*)(void)>> Extensions = {
    PUSH_EXTENSION("after_compilation", &on_easy),
    PUSH_EXTENSION("exercise_begin", &display_exercise_image),
    PUSH_EXTENSION("outer_extensions", &calculate_total_session_time),
    PUSH_EXTENSION("outer_extensions", &calculate_total_session_reps),
    PUSH_EXTENSION("after_compilation", &reverse_exercises),
    PUSH_EXTENSION("after_compilation", &on_bmode),
    PUSH_EXTENSION("after_compilation", &on_dummy),
    PUSH_EXTENSION("after_compilation", &on_warmup),
    PUSH_EXTENSION("after_compilation", &on_setall)
    // PUSH_EXTENSION("outer_extensions", &display_end_results)
    // PUSH_EXTENSION("round_end", &reverse_mode_activate),
    // PUSH_EXTENSION("outer_extensions", &describe_all_exercises)
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
