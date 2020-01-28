#include "converter.hh"

Converter::Converter(string path_in, string path_out)
: in(path_in), out(path_out)
{}

bool Converter::convert() {
    fipImage image_in = fipImage();
    image_in.load(this->in.c_str());
    if (!image_in.isValid()) {
        cerr << this->in << ": File is invalid" << endl;
        return false;
    }
    unsigned bpp = image_in.getBitsPerPixel();
    vector<vector<char>> m = this->map_image(image_in);
    if (!this->pooling(m, 101)) {
        cerr << "Invalid pooling k" << endl;
        return false;
    }
    fipImage image_out = this->generate_image(m, bpp);
    image_out.save(FIF_PNG, this->out.c_str());
    return true;
}

char Converter::convert_color(RGBQUAD& color) {
    unsigned threshold = 127;
    char colors[] = {'K', 'B', 'G', 'C', 'R', 'M', 'Y', 'W'};
    unsigned index = 0;
    if (color.rgbRed > threshold)
        index += 4;
    if (color.rgbGreen > threshold)
        index += 2;
    if (color.rgbBlue > threshold)
        index += 1;
    return colors[index];
}

RGBQUAD Converter::convert_code(char c) {
    char colors[] = {'K', 'B', 'G', 'C', 'R', 'M', 'Y', 'W'};
    unsigned index = 0;
    RGBQUAD color;
    while (index < 8 && colors[index] != c)
        index++;
    if (index == 8)
        return color;
    color.rgbRed = index < 4 ? 0 : 255;
    color.rgbGreen = (index % 4) < 2 ? 0 : 255;
    color.rgbBlue = index % 2 == 0 ? 0 : 255;
    return color; 
}

vector<vector<char>> Converter::map_image(const fipImage& image) {
    unsigned width = image.getWidth();
    unsigned height = image.getHeight();
    vector<vector<char>> m = vector<vector<char>>(width, vector<char>(height));
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color;
            image.getPixelColor(i, j, &color);
            m[i][j] = this->convert_color(color);
        }
    }
    return m;
}

fipImage Converter::generate_image(const vector<vector<char>>& m, unsigned bpp) {
    unsigned width = m.size();
    unsigned height = m[0].size();
    fipImage ret = fipImage(FIT_BITMAP, width, height, bpp);
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color = this->convert_code(m[i][j]);
            ret.setPixelColor(i, j, &color);
        }
    }
    return ret;
}

bool Converter::pooling(vector<vector<char>>& m, int k) {
    int width = m.size();
    int height = m[0].size();
    vector<vector<char>> ret = vector<vector<char>>(width, vector<char>(height));
    if (k % 2 == 0 || k < 0)
        return false;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            map<char, int> keys = {{'K', 0}, {'B', 0}, {'G', 0}, {'C', 0},
                                   {'R', 0}, {'M', 0}, {'Y', 0}, {'W', 0}};
            for (int x = -k/2; x <= k/2; ++x) {
                for (int y = -k/2; y <= k/2; ++y){
                    if (i + x >= 0 && i + x < width && j + y >= 0 && j + y < height){
                        keys[m[i+x][j+y]] += 1;
                    }
                }
            }
            char mcolor = 0;
            int marg = 0;
            for (auto it = keys.begin(); it != keys.end(); ++it) {
                if ((*it).second > marg) {
                    marg = (*it).second;
                    mcolor = (*it).first;
                }
            }
            ret[i][j] = mcolor;
        }
    }
    m = ret;
    return true;
}