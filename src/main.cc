#include <iostream>
#include "image/converter.hh"

using namespace std;

int main(int argc, char **argv) {
    FreeImage_Initialise();
    if (argc != 3) {
        cerr << argv[0] << ": Usage: " << argv[0] << " image_input.png image_output.png" << endl;
        FreeImage_DeInitialise();
        return 1;
    }
    Converter c = Converter(argv[1], argv[2]);
    if (!c.convert()) {
        FreeImage_DeInitialise();
        return 1;
    }
    FreeImage_DeInitialise();
    return 0;
}