#pragma once
#include <FreeImagePlus.h>
#include <cmath>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Converter {

public:
    Converter(string path_in, string path_out, unsigned colors, unsigned kernel_size);

    bool convert(); // converts the image
    double distance(RGBQUAD& color, int cluster); // get cluster distance
    int get_cluster(RGBQUAD& color); // gets the cluster of a color

    vector<vector<int>> map_image(); // maps a image into colors
    fipImage generate_image(const vector<vector<int>>& m, unsigned bpp); //generates a image from a mapping

    bool pooling(vector<vector<int>>& m, int k); // set all pixels to the majority of what's around them
    bool average_pooling(int k); // average pooling of image (blurring)
    
private:
    string out;
    unsigned kernel_size;
    fipImage image;
    vector<RGBQUAD> clusters;
};