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
    unsigned width = image_in.getWidth();
    unsigned height = image_in.getHeight();
    unsigned bpp = image_in.getBitsPerPixel();
    fipImage image_out = fipImage(FIT_BITMAP, width, height, bpp);
    for (unsigned i = 0; i < width; ++i) {
        for (unsigned j = 0; j < height; ++j) {
            RGBQUAD color;
            image_in.getPixelColor(i, j, &color);
            this->convert_color(color);
            image_out.setPixelColor(i, j, &color);
        }
    }
    image_out.save(FIF_PNG, this->out.c_str());
    return true;
}

void Converter::convert_color(RGBQUAD& color) {
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
}