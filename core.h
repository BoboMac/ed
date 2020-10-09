// Only for includes and defines
#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>
#define UINT unsigned int
#define UCHAR unsigned char

std::string ReadFile(std::string filename) {
    char* result;
    FILE* file = fopen(filename.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    UINT s = ftell(file);
    result = (char*)malloc((s + 1) * sizeof(char));
    fseek(file, 0, SEEK_SET);
    fread(result, sizeof(char), s, file);
    fclose(file);
    result[s] = 0;
    return result;
}
