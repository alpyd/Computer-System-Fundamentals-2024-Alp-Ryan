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
        void printSummaryInfo();


    private:
        uint32_t readTag(uint32_t memoryAddress);
        uint32_t readIndex(uint32_t memoryAddress);
        bool load(uint32_t memoryAddress, bool isDirty);
        bool store(uint32_t memoryAddress);
        uint32_t chooseSlotIndex(Set& set);
        void handleLoadMiss(Set& set, uint32_t tag, bool isDirty);
        void handleStoreMiss(uint32_t memoryAddress);

        Cache cache;
        int numOfSets;
        int setSize;
        int blockSize;
        bool writeAllocate;
        bool writeThrough;
        bool evictionLRU;
        int totLoads; 
        int totStores; 
        int totCycles; 
        int LTotHits; 
        int LTotMisses; 
        int STotHits; 
        int STotMisses; 
        int timestamp;
};


#endif 