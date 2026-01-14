#include "cli.h"
#include <iostream>
#include <exception>

int main(int argc, char** argv) {
    try {
        ParseCLI(argc, argv);
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}
