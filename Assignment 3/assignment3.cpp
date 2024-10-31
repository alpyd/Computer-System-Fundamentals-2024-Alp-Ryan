#include <iostream>
#include <sstream>
#include "cachesimulator.h"
#include <cstring>

// Ensure that the inputted number is a valid positive power of two
bool validPowerOfTwo(int num){
    return num > 0 && (num & (num - 1)) == 0;
}


bool stringInputsValid(char* argv[]){
    // Ensure that either write-allocate or no-write-allocate parameter is provided and throw an error if neither provided
    bool isWriteAllocate = false;
    if (strcmp(argv[4], "write-allocate") == 0) {
        isWriteAllocate = true;
    } else if (strcmp(argv[4], "no-write-allocate") == 0) {
        isWriteAllocate = false;
    } else {
        std::cerr << "Error - Must either indicate write-allocate or no-write-allocate" << std::endl;
        return false;
    }

    // Ensure that either write-through or write-back parameter is provided and throw an error if neither provided
    bool isWriteThrough = false;
    if (strcmp(argv[5], "write-through") == 0) {
        isWriteThrough = true;
    } else if (strcmp(argv[5], "write-back") == 0) {
        isWriteThrough = false;
    } else {
        std::cerr << "Error - Must either indicate write-through or write-back" << std::endl;
        return false;
    }

    // Ensure that the cache combination of no-write-allocate with write-back is specified. Wouldn't write cache to store
    if (!isWriteThrough && !isWriteAllocate) {
        std::cerr << "Error - Does not make sense to combine no-write-allocate with write-back." << std::endl;
        return false;
    }

    // Ensure that the provided eviciton policy is either fifo or lru
    if (strcmp(argv[6], "fifo") != 0 && strcmp(argv[6], "lru") != 0) {
        std::cerr << "Error - Eviction policy must either be fifo or lru" << std::endl;
        return false;
    }

    return true;
}

bool numbersValid(char* argv[]) {
    // Ensure that the numerical inputs are correct
    for (int i = 1; i <= 3; i++) {

        // Try to convert the inputted value to a number and throw an error if unsuccessful
        int value;
        try {
            value = std::stoi(argv[i]);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error - Invalid number format for argument " << i << std::endl;
            return false;
        }

        // Throw an error if the number is not a valid power of two
        if (value <= 0 || !validPowerOfTwo(value)) {
            std::cerr << "Error - Argument " << i << " must be a positive power of two" << std::endl;
            return false;
        }
    }

    // Specifically make sure that the block size is at least 4 bytes
    if (std::stoi(argv[3]) < 4) {
        std::cerr << "Error - Block size must be at least 4 bytes" << std::endl;
        return false;
    }

    return true;
}

bool parametersValid(int argc, char* argv[]) {
    // Check if user has 6 parameters
    if (argc != 7) {
        std::cerr << "Error - You must input 6 parameters" << std::endl;
        return false;
    }

    // Ensure that the number and string parameters are both valid
    if (numbersValid(argv) && stringInputsValid(argv)) {
        return true;
    }

    return false;
}

int main(int argc, char *argv[]) {
    // argc: number of command-line arguments
    // argv: array of C-style strings (char*), representing each argument

    // Ensure that the provided parameters are valid. Otherwise, return null
    if (!parametersValid(argc, argv)) {
        return 1;
    }

    // Print out the number of arguments
    std::cout << "Number of arguments: " << argc << std::endl;

    // Initialize the user parameters
    int numSets = std::stoi(argv[1]);
    int numBlocks = std::stoi(argv[2]);
    int blockMem = std::stoi(argv[3]);
    bool writeAllocate = (strcmp(argv[4], "write-allocate") == 0);
    bool writeThrough = (strcmp(argv[5], "write-through") == 0);
    bool evictionLRU = (strcmp(argv[6], "lru") == 0);

    // Instantiate the CacheSimulator class
    CacheSimulator csim(numSets, numBlocks, blockMem, writeAllocate, writeThrough, evictionLRU);

    // Read in the commands using std::cin to get the line and istringstream to parse the command
    std::string inputLine;
    while (std::getline(std::cin, inputLine)) {
        std::istringstream commandPrompt = std::istringstream(inputLine);
        char command;
        std::string memoryAddressStr;
        uint32_t memoryAddress;

        // Read the command ('l' or 's') and return error if otherwise
        if (!(commandPrompt >> command) || (command != 'l' && command != 's')) {
            std::cerr << "Error: Invalid command format." << std::endl;
            // Return 1 for error
            return 1;
        }

        // Read the memory address as a hexadecimal string and return error if unsuccessful
        if (!(commandPrompt >> memoryAddressStr)) {
            std::cerr << "Error: Invalid memory address format." << std::endl;

            // Return 1 for error
            return 1;
        }

        // Convert the memory address string (e.g., "0x0000AA40") to an integer
        memoryAddress = std::stoul(memoryAddressStr, nullptr, 16);

        // Pass the command and memory address to the simulator
        csim.executeCommand(command, memoryAddress);
    }

    // Print the summary info once all commands have been executed
    csim.printSummaryInfo();

    // Return 0 for success
    return 0;
}