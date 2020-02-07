#include <iostream>
#include "converter/converter.hh"
#include "option/option.hh"

using namespace std;

int main(int argc, char **argv) {
    FreeImage_Initialise();
    Option option = Option();
    if (!option.parse(argv, argc)) {
        cerr << argv[0] << ": Usage: " << argv[0] << " [-c colors][-k kernel_size] image_input.png image_output.png" << endl;
        FreeImage_DeInitialise();
        return 1;
    }
    Converter c = Converter(option.get_in(), option.get_out(), option.get_colors(), option.get_kernel_size());
    if (!c.convert()) {
        FreeImage_DeInitialise();
        return 1;
    }
    FreeImage_DeInitialise();
    return 0;
}