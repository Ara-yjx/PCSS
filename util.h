#include <string>
#include <stdlib.h>

#ifndef UTIL_H
#define UTIL_H

std::string fileToString(std::string filename) {
    std::ifstream f(filename);
    std::string str;
    if(f) {
        std::ostringstream ss;
        ss << f.rdbuf(); 
        str = ss.str();
    } else {
        std::cout<<"ERROR Opening file: "<<filename<<std::endl;
    }
    return str;
}


float random(float x, float y) {
    return ((float) rand() / (RAND_MAX)) * (y-x) + x ;
}

#endif