#include "converter.hh"

Converter::Converter(string path_in, string path_out)
: in(path_in), out(path_out)
{}

bool Converter::convert() const {
    FIBITMAP *image_in = FreeImage_Load(FIF_PNG, this->in.c_str());
    unsigned width = FreeImage_GetWidth(image_in);
    unsigned height = FreeImage_GetHeight(image_in);
    unsigned bpp = FreeImage_GetBPP(image_in);
    FIBITMAP *image_out = FreeImage_Allocate(width, height, bpp);
    if (image_in == nullptr) {
        cerr << this->in << ": File not found" << endl;
        return false;
    }
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color;
            FreeImage_GetPixelColor(image_in, i, j, &color);
            if (color.rgbRed > 127)
                color.rgbRed = 255;
            else
                color.rgbRed = 0;
            if (color.rgbGreen > 127)
                color.rgbGreen = 255;
            else
                color.rgbGreen = 0;
            if (color.rgbBlue > 127)
                color.rgbBlue = 255;
            else
                color.rgbBlue = 0;
            FreeImage_SetPixelColor(image_out, i, j, &color);
        }
    }
    FreeImage_Save(FIF_PNG, image_out, this->out.c_str());
    return true;
}