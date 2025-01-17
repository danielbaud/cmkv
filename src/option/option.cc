#include "option.hh"

Option::Option() {
    this->colors = 8;
    this->kernel_size = 7;
    this->in = "";
    this->out = "";
}

bool Option::parse(char **argv, int argc) {
    if (argc < 3)
        return false;
    for (int i = 1; i < argc; ++i) {
        string arg = string(argv[i]);
        if (arg[0] == '-') {
            if (arg == "-c" || arg == "--colors") {
                i++;
                if (i == argc) {
                    cerr << argv[0] << ": color argument not found" << endl;
                    return false;
                }
                this->colors = atoi(argv[i]);
            }
            else if (arg == "-s" || arg == "--smoothen") {
                i++;
                if (i == argc) {
                    cerr << argv[0] << ": smoothen argument not found" << endl;
                    return false;
                }
                this->kernel_size = 2 * atoi(argv[i]) + 1;
            }
            else {
                cerr << argv[0] << ": " << arg << ": unknown option" << endl;
                return false;
            }
        }
        else {
            if (this->in == "")
                this->in = arg;
            else if (this->out == "")
                this->out = arg;
            else {
                cerr << argv[0] << ": Too much arguments" << endl;
                return false;
            }
        }
    }
    if (this->in == "" || this->out == "") {
        cerr << argv[0] << ": Too few arguments" << endl;
        return false;
    }
    return true;
}