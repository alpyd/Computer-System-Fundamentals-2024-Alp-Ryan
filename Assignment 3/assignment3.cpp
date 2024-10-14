#include <iostream>
#include <sstream>
#include "cachesimulator.h"

int main(int argc, char *argv[]) {
    // argc: number of command-line arguments
    // argv: array of C-style strings (char*), representing each argument

    if(!parametersValid(argc, argv)){
        return 1;
    }

    std::cout << "Number of arguments: " << argc << std::endl;

    int numSets = std::stoi(argv[1]);
    int numBlocks = std::stoi(argv[2]);
    int blockMem = std::stoi(argv[3]);
    bool writeAllocate = (argv[4] == "write-allocate");
    bool writeThrough = (argv[5] == "write-through");

    CacheSimulator csim(numSets, numBlocks, blockMem, writeAllocate, writeThrough);

    std::string inputLine;
    while(std::getline(std::cin, inputLine)){
        std::istringstream commandPrompt = std::istringstream(inputLine);
        char command;
        uint32_t memoryAddress;

        commandPrompt >> command;
        commandPrompt >> memoryAddress;

        csim.executeCommand(command, memoryAddress);
    }

    csim.printSummaryInfo();
    return 0;
}

bool isPositiveNumber(const char* num) {
    if(num[0] == '\0'){
        return false;
    }
    
    // Iterate through the string to check if all characters are digits
    int digitNumber = 0;
    while (num[digitNumber] != '\0'){
        if (num[digitNumber] < '0' || num[digitNumber] > '0'){
            return false;
        }
        digitNumber++;
    }
    return true;
}

bool validPowerOfTwo(int num){
    return num > 0 && (num & (num - 1)) == 0;
}

bool parametersValid(int argc, char* argv[]){
    if(argc != 7){
        std::cerr << "Error - You must input 6 parameters" << std::endl;
        return false;
    }
    for(int i = 1; i <= 3; i++){
        if(!isPositiveNumber(argv[i]) || !validPowerOfTwo(std::stoi(argv[i]))) {
            std::cerr << "Error - You must input positive power of two" << std::endl;
            return false;
        }
    }
    if(std::stoi(argv[3]) < 4){
        std::cerr << "Block size must be at least 4 bytes" << std::endl;
        return false;
    }

    // TODO: NEED TO COMPARE STRINGS
}
