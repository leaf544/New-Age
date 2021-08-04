#ifndef EXERCISE_HPP
#define EXERCISE_HPP

/* STD INCLUDES */
#include <iostream>
#include <string>

using std::cout;
using std::endl;

/* INCLUDES */
#include "macros.h"
#include "data.h"

class Exercise {
public:
    std::string name;
    char freestyle;
    int sets;
    int reps;
    int hold;
    int ahold;

    // Default constructor
    Exercise(std::string n) {
        this->name = n;
        this->freestyle = DEFAULT_FREESTYLE;
        this->sets = DEFAULT_SETS;
        this->reps = DEFAULT_REPS;
        this->hold = DEFAULT_HOLD;
        this->ahold = DEFAULT_AHOLD;
    }
    
    // Constructor with options
    Exercise(std::string n, char f, int s, int r, int h, int a) {
        this->name= n;
        this->freestyle = f;
        this->sets = s;
        this->reps = r;
        this->hold = h;
        this->ahold = a;
    }
    
    void Describe() {
        cout << "# Name: " << this->name << endl;
        cout << "# Freestyle: " << this->freestyle << endl;
        cout << "# Sets: " << this->sets << endl;
        cout << "# Reps: " << this->reps << endl;
        cout << "# Hold: " << this->hold << "s" << endl;
        cout << "# Ahold: " << this->ahold << "s" << endl;
        cout << "# Time: " << CalculateTime() << " minute(s)" << endl << endl;
    }

    long long CalculateTime() {
        return ((this->sets * (this->reps * this->hold) + (this->reps * ahold)) / 60);
    }
    
};

#endif
