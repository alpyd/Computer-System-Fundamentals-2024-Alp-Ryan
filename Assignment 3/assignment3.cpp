#include <iostream>
#include <sstream>
#include "cachesimulator.h"
#include <cstring>

bool validPowerOfTwo(int num){
    return num > 0 && (num & (num - 1)) == 0;
}

bool parametersValid(int argc, char* argv[]) {
    if (argc != 7) {
        std::cerr << "Error - You must input 6 parameters" << std::endl;
        return false;
    }

    for (int i = 1; i <= 3; i++) {
        int value;
        try {
            value = std::stoi(argv[i]);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error - Invalid number format for argument " << i << std::endl;
            return false;
        }

        if (value <= 0 || !validPowerOfTwo(value)) {
            std::cerr << "Error - Argument " << i << " must be a positive power of two" << std::endl;
            return false;
        }
    }

    if (std::stoi(argv[3]) < 4) {
        std::cerr << "Error - Block size must be at least 4 bytes" << std::endl;
        return false;
    }

    bool isWriteAllocate = false;
    if (strcmp(argv[4], "write-allocate") == 0) {
        isWriteAllocate = true;
    } else if (strcmp(argv[4], "no-write-allocate") == 0) {
        isWriteAllocate = false;
    } else {
        std::cerr << "Error - Must either indicate write-allocate or no-write-allocate" << std::endl;
        return false;
    }

    bool isWriteThrough = false;
    if (strcmp(argv[5], "write-through") == 0) {
        isWriteThrough = true;
    } else if (strcmp(argv[5], "write-back") == 0) {
        isWriteThrough = false;
    } else {
        std::cerr << "Error - Must either indicate write-through or write-back" << std::endl;
        return false;
    }

    if (!isWriteThrough && !isWriteAllocate) {
        std::cerr << "Error - Does not make sense to combine no-write-allocate with write-back." << std::endl;
        return false;
    }

    if (strcmp(argv[6], "fifo") != 0 && strcmp(argv[6], "lru") != 0) {
        std::cerr << "Error - Eviction policy must either be fifo or lru" << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    // argc: number of command-line arguments
    // argv: array of C-style strings (char*), representing each argument

    if (!parametersValid(argc, argv)) {
        return 1;
    }

    std::cout << "Number of arguments: " << argc << std::endl;

    int numSets = std::stoi(argv[1]);
    int numBlocks = std::stoi(argv[2]);
    int blockMem = std::stoi(argv[3]);
    bool writeAllocate = (strcmp(argv[4], "write-allocate") == 0);
    bool writeThrough = (strcmp(argv[5], "write-through") == 0);
    bool evictionLRU = (strcmp(argv[6], "lru") == 0);

    CacheSimulator csim(numSets, numBlocks, blockMem, writeAllocate, writeThrough, evictionLRU);

    std::string inputLine;
    while (std::getline(std::cin, inputLine)) {
        std::istringstream commandPrompt = std::istringstream(inputLine);
        char command;
        std::string memoryAddressStr;
        uint32_t memoryAddress;

        // Read the command ('l' or 's')
        if (!(commandPrompt >> command) || (command != 'l' && command != 's')) {
            std::cerr << "Error: Invalid command format." << std::endl;
            return 1;
        }

        // Read the memory address as a hexadecimal string
        if (!(commandPrompt >> memoryAddressStr)) {
            std::cerr << "Error: Invalid memory address format." << std::endl;
            return 1;
        }

        // Convert the memory address string (e.g., "0x0000AA40") to an integer
        memoryAddress = std::stoul(memoryAddressStr, nullptr, 16);

        // Pass the command and memory address to the simulator
        csim.executeCommand(command, memoryAddress);
    }

    csim.printSummaryInfo();
    return 0;
}
