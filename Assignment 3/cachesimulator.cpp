#include "cache.h"
#include "cachesimulator.h"
#include <iostream>
#include <cmath>

CacheSimulator::CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU)
    : numOfSets(numOfSets), setSize(setSize), blockSize(blockSize), 
      writeAllocate(writeAllocate), writeThrough(writeThrough), evictionLRU(evictionLRU), 
      totCycles(0), STotMisses(0), timestamp(0) {

    for(int i = 0; i < numOfSets; i++){
        Set newSet;
        for (int j = 0; j < setSize; j++){
            Slot newSlot;
            newSlot.valid = false;
            newSlot.dirty = false;
            newSlot.access_ts = 0; // Initialize access_ts
            newSlot.load_ts = 0;   // Initialize load_ts
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

uint32_t CacheSimulator::readTag(uint32_t memoryAddress) {
    int numBitsOffset = std::log2(blockSize);  // Number of bits for the block offset
    int numBitsIndex = std::log2(numOfSets);   // Number of bits for the index

    return memoryAddress >> (numBitsOffset + numBitsIndex);
}

uint32_t CacheSimulator::readIndex(uint32_t memoryAddress) {
    int numBitsOffset = std::log2(blockSize);  // Number of bits for the block offset
    int numBitsIndex = std::log2(numOfSets);   // Number of bits for the index

    return (memoryAddress >> numBitsOffset) & ((1 << numBitsIndex) - 1);
}


uint32_t CacheSimulator::chooseSlotIndex(Set& set){
    for(uint32_t i = 0; i < set.slots.size(); i++){
        if(!set.slots.at(i).valid){
            return i;
        }
    }

    int chosenIndex = 0;
    if(evictionLRU) {
        for(uint32_t i = 0; i < set.slots.size(); i++){
            if(set.slots.at(i).access_ts < set.slots.at(chosenIndex).access_ts){
                chosenIndex = i;
            }
        }
    } else {
        for(uint32_t i = 0; i < set.slots.size(); i++){
            if(set.slots.at(i).load_ts < set.slots.at(chosenIndex).load_ts) {
                chosenIndex = i;
            }
        }
    }

    return chosenIndex;
}

bool CacheSimulator::load(uint32_t memoryAddress, bool isDirty){ //reading from cache
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readIndex(memoryAddress);
    Set &set = cache.sets[index];
    for (unsigned int i = 0; i < set.slots.size(); i++){
        Slot &slot = set.slots.at(i);
        if(slot.valid && slot.tag == tag){    
            
            slot.access_ts = ++timestamp;
            if(isDirty){
                slot.dirty = true;
            }
            totCycles++;
            return true;
        }
    }

    handleLoadMiss(set, tag, isDirty);
    return false;
    
}

void CacheSimulator::handleLoadMiss(Set& set, uint32_t tag, bool isDirty){ //tag for index not found, get data from memory and load into cache
    Slot &chosenSlot = set.slots.at(chooseSlotIndex(set));

    if(!evictionLRU){
        chosenSlot.load_ts = ++timestamp;
    }

    if(chosenSlot.valid && chosenSlot.dirty){
        totCycles += ((blockSize / 4)  * 100);
    }

    chosenSlot.tag = tag;
    chosenSlot.valid = true;
    chosenSlot.dirty = isDirty;
    chosenSlot.access_ts = ++timestamp;
    totCycles += ((blockSize / 4) * 100);
}

bool CacheSimulator::store(uint32_t memoryAddress){ //writing to cache
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readIndex(memoryAddress);
    Set &set = cache.sets[index];
    
    // Check if the tag is already in the cache (cache hit)
    for (unsigned int i = 0; i < set.slots.size(); i++){
        Slot &slot = set.slots.at(i);  // Use a reference to modify the slot directly
        if(slot.valid && slot.tag == tag){
            timestamp++;
            slot.access_ts = timestamp;

            // Perform the appropriate action based on writeThrough parameter
            if (writeThrough) {
                totCycles += 100; // Handle cycles
                slot.dirty = false;
            } else {
                slot.dirty = true; // Mark the slot as dirty since we're storing data
            } 
            
            totCycles++; // Increment total cycles for the operation
            return true;
        }
    }
    
    handleStoreMiss(memoryAddress); // Handle a store miss
    return false; // Return false if the store was unsuccessful
}


void CacheSimulator::handleStoreMiss(uint32_t memoryAddress){
    // On a store miss, load the relevant block into the cache if write-allocate
    if (writeAllocate) {
        load(memoryAddress, true); // Load and mark as dirty
    } else {
        // No-write-allocate: Write directly to memory without cache modification
        totCycles += 100; // Writing to memory directly (100 cycles)
    }
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