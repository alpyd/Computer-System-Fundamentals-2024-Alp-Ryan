#ifndef CACHESIMULATOR_H
#define CACHESIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cache.h"

class CacheSimulator {
    public:
        CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU);
        void executeCommand(char command, uint32_t memoryAddress);
        bool load(uint32_t memoryAddress, bool isDirty);
        bool store(uint32_t memoryAddress);
        void printSummaryInfo();

    private:
        Cache cache;

        int totalLoads; 
        int totalStores; 
        int loadHits; 
        int loadMisses; 
        int storeHits; 
        int storeMisses; 
        int totalCycles; 
};


#endif 