#pragma once
#include <FreeImagePlus.h>
#include <string>
#include <iostream>

using namespace std;

class Converter {

public:
    Converter(string path_in, string path_out);
    bool convert();
    void convert_color(RGBQUAD& color);

private:
    string in;
    string out;
};