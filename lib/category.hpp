#ifndef CATEGORY_H
#define CATEGORY_H

/* STD */
#include <iostream>
#include <vector>
#include <vector>
#include <iterator>
#include <string>
#include <map>
#include <sstream>
#include <fstream>

/* C */
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h>
#include <string.h>

using std::cout;
using std::endl;

class Category {
public:
    std::map<std::string, std::string> Variables;
    bool hasVariables = false;
    
    std::string FetchValue (std::string iden) {
        return this->Variables[iden] != "" ? this->Variables[iden] : "0";
    }
    
    int FetchValueInt (std::string iden) {
        return atoi(FetchValue(iden).c_str());
    }

    void ListVariables () {
        for (auto it = this->Variables.begin(); it != this->Variables.end(); it++) {
            cout << "IDEN: " << it->first << " / " << "VALUE: " << it->second << endl;
        }
    }
    
};

#endif
