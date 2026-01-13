#include "cli.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        parseCLI(argc, argv);
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}
