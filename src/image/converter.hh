#pragma once
#include <FreeImage.h>
#include <string>
#include <iostream>

using namespace std;

class Converter {

public:
    Converter(string path_in, string path_out);
    bool convert() const;

private:
    string in;
    string out;
};