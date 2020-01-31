#pragma once
#include <FreeImagePlus.h>
#include <cmath>
#include <string>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Converter {

public:
    Converter(string path_in, string path_out);

    bool convert(); // converts the image
    double distance(RGBQUAD& color, int cluster); // get cluster distance
    int get_cluster(RGBQUAD& color); // gets the cluster of a color

    vector<vector<int>> map_image(const fipImage& image); // maps a image into colors
    fipImage generate_image(const vector<vector<int>>& m, unsigned bpp); //generates a image from a mapping

    bool pooling(vector<vector<int>>& m, int k); // set all pixels to the majority of what's around them
    bool average_pooling(fipImage& image, int k); // average pooling of image (blurring)

    void print_clusters();

private:
    string out;
    fipImage image;
    vector<RGBQUAD> clusters;
};