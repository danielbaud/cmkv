#pragma once
#include <FreeImagePlus.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Converter {

public:
    Converter(string path_in, string path_out);
    bool convert();
    char convert_color(RGBQUAD& color);
    vector<vector<char>> map_image(const fipImage& image);
    fipImage generate_image(const vector<vector<char>>& m, unsigned bpp);
    RGBQUAD convert_code(char c);
    bool pooling(vector<vector<char>>& m, int k);
    bool average_pooling(fipImage& image, int k);

private:
    string in;
    string out;
};