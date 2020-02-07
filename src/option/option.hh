#pragma once
#include <string>
#include <iostream>

using namespace std;

class Option {

public:
    Option();
    bool parse(char **argv, int argc);
    string get_in() {return this->in;}
    string get_out() {return this->out;}
    unsigned get_kernel_size() {return this->kernel_size;}
    unsigned get_colors() {return this->colors;}

private:
    string in;
    string out;
    unsigned kernel_size;
    unsigned colors;
};