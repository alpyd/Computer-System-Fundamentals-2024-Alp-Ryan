#ifndef CACHESIMULATOR_H
#define CACHESIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "cache.h"

class CacheSimulator {
    public:
        // Constructor that takes in user-defined parameters at the start
        CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU);
        
        // Executes either a load or store command for a given memory address
        void executeCommand(char command, uint32_t memoryAddress);

        // Prints the summary information in the given format once there are no more commands
        void printSummaryInfo();


    private:
        // Read and output the tag from a memory address
        uint32_t readTag(uint32_t memoryAddress);

        // Read and output the index from a memory address
        uint32_t readIndex(uint32_t memoryAddress);

        // Simulate a memory load from the cache
        bool load(uint32_t memoryAddress, bool isDirty);

        // Simulate a memory store into the cache
        bool store(uint32_t memoryAddress);

        // Choose a slot to load into based on a given eviction policy
        uint32_t chooseSlotIndex(Set& set);

        // Helper function to define what happens when a load miss occurs
        void handleLoadMiss(Set& set, uint32_t tag, bool isDirty);

        // Helper function to define what happens when a store miss occurs
        void handleStoreMiss(uint32_t memoryAddress);

        // Cache object used in CacheSimulator
        Cache cache;

        // Number of sets defined by user
        int numOfSets;

        // Set size defined by user
        int setSize;

        // Block size defined by user
        int blockSize;

        // Boolean to indicate whether write-allocate or no-write-allocate
        bool writeAllocate;

        // Boolean to indicate whether write-through or write-back
        bool writeThrough;

        // Boolean to indicate FIFO or LRU eviction
        bool evictionLRU;

        // Tracker for total loads
        int totLoads; 

        // Tracker for total stores
        int totStores; 

        // Tracker for total cycles
        int totCycles; 

        // Tracker for total load hits
        int LTotHits; 

        // Tracker for total load misses
        int LTotMisses; 

        // Tracker for total store hits
        int STotHits; 

        // Tracker for total store misses
        int STotMisses; 

        // Tracker for the current timestamp of the CacheSimulator
        int timestamp;
};


#endif 