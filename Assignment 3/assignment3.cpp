#include <iostream>

int main(int argc, char *argv[]) {
    // argc: number of command-line arguments
    // argv: array of C-style strings (char*), representing each argument

    std::cout << "Number of arguments: " << argc << std::endl;

    // print out arguments
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    return 0;
}
