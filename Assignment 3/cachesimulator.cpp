#include "cache.h"
#include "cachesimulator.h"
#include <iostream>
#include <cmath>

CacheSimulator::CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU){
    int totLoads = 0; 
    int totStores = 0; 
    int LTotHits = 0; 
    int LTotMisses = 0; 
    int STotHits = 0; 
    int STotMisses = 0; 
    int totCycles = 0; 
    int timestamp = 0;

    for(int i = 0; i < numOfSets; i++){
        Set newSet;
        for (int j = 0; j < setSize; j++){
            Slot newSlot;
            newSet.slots.push_back(newSlot);
        }
        cache.sets.push_back(newSet);
    }
}

void CacheSimulator::executeCommand(char command, uint32_t memoryAddress){
    if(command == 's') {
        totStores++;
        if(store(memoryAddress)){
            STotHits++;
        } else {
            STotMisses++;
        }
    } else if(command == 'l'){
        totLoads++;
        if(load(memoryAddress, false)){
            LTotHits++;
        } else {
            LTotMisses++;
        }
    } 
}

uint32_t CacheSimulator::readTag(uint32_t memoryAddress){
    int numBitsOffset = std::log(blockSize)/std::log(2);
    int numBitsIndex = std::log(numOfSets)/std::log(2);
    return memoryAddress >> (numBitsOffset + numBitsIndex);
}

uint32_t CacheSimulator::readIndex(uint32_t memoryAddress){
    int numBitsOffset = std::log(blockSize)/std::log(2);
    int numBitsIndex = std::log(numOfSets)/std::log(2);
    return (memoryAddress >> numBitsOffset) & ((1 << numBitsIndex) - 1);
}

bool CacheSimulator::load(uint32_t memoryAddress, bool isDirty){
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readTag(memoryAddress);
    Set &set = cache.sets[index];
    for (int i = 0; i < set.slots.size(); i++){
        Slot slot = set.slots.at(i);
        if(slot.valid && slot.tag == tag){
            timestamp++;
            slot.access_ts = timestamp;
            if(isDirty){
                slot.dirty = true;
            }
            totCycles++;
            return true;
        }
    }
    // What to do if it misses 

}

bool CacheSimulator::store(uint32_t memoryAddress){
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readTag(memoryAddress);
    Set &set = cache.sets[index];
    for (int i = 0; i < set.slots.size(); i++){
        Slot slot = set.slots.at(i);
        if(slot.valid && slot.tag == tag){
            timestamp++;
            slot.access_ts = timestamp;
            if(writeThrough){
                load(memoryAddress, false);
                totCycles += 100; // Handle cycles
            } else {
                load(memoryAddress, true);
                // handle cycles
            }
            totCycles++;
            return true;
        }
    }
    // What to do if it misses

}

void CacheSimulator::printSummaryInfo(){
    std::cout << "Total loads: " << totLoads << std::endl;
    std::cout << "Total stores: " << totStores << std::endl;
    std::cout << "Load hits: " << LTotHits << std::endl;
    std::cout << "Load misses: " << LTotMisses << std::endl;
    std::cout << "Store hits: " << STotHits << std::endl;
    std::cout << "Store misses: " << STotMisses << std::endl;
    std::cout << "Total cycles: " << totCycles << std::endl;
}