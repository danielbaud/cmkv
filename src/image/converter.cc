#include "converter.hh"

Converter::Converter(string path_in, string path_out)
: in(path_in), out(path_out)
{}

bool Converter::convert() const {
    cout << "Converting " << this->in << " to " << this->out << endl;
    FIBITMAP *image_in = FreeImage_Load(FIF_PNG, this->in.c_str());
    unsigned width = FreeImage_GetWidth(image_in);
    unsigned height = FreeImage_GetHeight(image_in);
    FIBITMAP *image_out = FreeImage_Allocate(width, height, 1);
    if (image_in == nullptr) {
        cerr << this->in << ": File not found" << endl;
        return false;
    }
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color;
            FreeImage_GetPixelColor(image_in, i, j, &color);
            FreeImage_SetPixelColor(image_out, i, j, &color);
        }
    }
    FreeImage_Save(FIF_PNG, image_out, this->out.c_str());
    return true;
}