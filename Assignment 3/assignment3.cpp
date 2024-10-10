#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {
    // argc: number of command-line arguments
    // argv: array of C-style strings (char*), representing each argument

    std::cout << "Number of arguments: " << argc << std::endl;

    int numSets = std::stoi(argv[1]);
    int numBlocks = std::stoi(argv[2]);
    int blockMem = std::stoi(argv[3]);
    bool writeAllocate = (argv[4] == "write-allocate");
    bool writeThrough = (argv[5] == "write-through");

    std::string inputLine;
    while(std::getline(std::cin, inputLine)){
        std::istringstream commandPrompt = std::istringstream(inputLine);
        char command;
        uint32_t memoryAddress;

        commandPrompt >> command;
        commandPrompt >> memoryAddress;

        // Execute Cache Action
        
    }


    // Print Out Cache Arguments

    return 0;
}
