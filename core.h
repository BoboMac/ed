// Only for includes, defines and common functions
#pragma once

#include <iostream> // Casual output
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <algorithm> // std::find()
#include <stdlib.h> // exit()
#include <unistd.h> // access() for file checking
#include "gainput/gainput.h"
#define UINT unsigned int
#define UCHAR unsigned char

// C-style file reading (faster than C++ I/O streams)
std::string ReadFile(std::string filename) {
    char* result;
    FILE* file = fopen(filename.c_str(), "rb");
    if(!file) return "FILE_NOT_FOUND";
    fseek(file, 0, SEEK_END);
    UINT s = ftell(file);
    result = (char*)malloc((s + 1) * sizeof(char));
    fseek(file, 0, SEEK_SET);
    fread(result, sizeof(char), s, file);
    fclose(file);
    result[s] = 0;
    return result;
}

std::vector<std::string> ParseStrIntoVec(std::string str) {
    std::vector<std::string> result;
    std::string s;
    for(int i = 0; i < str.size(); i++) {
        if (str[i] == '\n') {
            result.push_back(s);
            s = "";
        }
        else {
            s += str[i];
        }
    }
    result.push_back(s);
    return result;
}
