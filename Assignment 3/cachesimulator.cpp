#include "cache.h"
#include "cachesimulator.h"
#include <iostream>
#include <cmath>

CacheSimulator::CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU)
    : numOfSets(numOfSets), setSize(setSize), blockSize(blockSize), writeAllocate(writeAllocate), writeThrough(writeThrough), evictionLRU(evictionLRU),
    totLoads(0), totStores(0), totCycles(0), LTotHits(0), LTotMisses(0), STotHits(0), STotMisses(0), timestamp(0)  {

    for(int i = 0; i < numOfSets; i++){
        Set newSet;
        for (int j = 0; j < setSize; j++){
            Slot newSlot;
            newSlot.valid = false;
            newSlot.dirty = false;
            newSet.slots.push_back(newSlot);
        }
        cache.sets.push_back(newSet);
    }
}

void CacheSimulator::executeCommand(char command, uint32_t memoryAddress){
    if(command == 's') {
        totStores++;
        if(store(memoryAddress)){ //track of store hits/misses
            STotHits++;
        } else {
            STotMisses++;
        }
    } else if(command == 'l'){ //track of load hits/misses
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
    for (unsigned int i = 0; i < set.slots.size(); i++){ //iterate over blocks in set
        Slot &slot = set.slots.at(i);
        if(slot.valid && slot.tag == tag){    
            
            slot.access_ts = ++timestamp;
            if(isDirty){
                slot.dirty = true;
            }
            totCycles++; //1 cycle per load operation
            return true;
        }
    }

    handleLoadMiss(set, tag, isDirty);
    return false;
    

}

void CacheSimulator::handleLoadMiss(Set& set, uint32_t tag, bool isDirty){
    Slot chosenSlot = set.slots.at(chooseSlotIndex(set));

    if(!evictionLRU){
        chosenSlot.load_ts = ++timestamp;
    }

    if(chosenSlot.valid && chosenSlot.dirty){ // add 100 cycles per 4 byte quantity
        totCycles += ((blockSize / 4)  * 100);
    }

    chosenSlot.tag = tag;
    chosenSlot.valid = true;
    chosenSlot.dirty = isDirty;
    chosenSlot.access_ts = ++timestamp;
    totCycles += ((blockSize / 4) * 100); //add 100 cycles per 4 byte quantity
}

uint32_t CacheSimulator::chooseSlotIndex(Set& set){
    for(std::vector<Slot>::size_type i = 0; i < set.slots.size(); i++){
        if(!set.slots.at(i).valid){
            return i;
        }
    }

    int chosenIndex = 0;
    if(evictionLRU) {
        for(std::vector<Slot>::size_type i = 0; i < set.slots.size(); i++){
            if(set.slots.at(i).access_ts < set.slots.at(chosenIndex).access_ts){
                chosenIndex = i;
            }
        }
    } else {
        for(std::vector<Slot>::size_type i = 0; i < set.slots.size(); i++){
            if(set.slots.at(i).load_ts < set.slots.at(chosenIndex).load_ts) {
                chosenIndex = i;
            }
        }
    }

    return chosenIndex;
}

bool CacheSimulator::store(uint32_t memoryAddress){
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readTag(memoryAddress);
    Set &set = cache.sets[index];
    for (std::vector<Slot>::size_type i = 0; i < set.slots.size(); i++){
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
    
    handleStoreMiss(memoryAddress);
    return false;

}

void CacheSimulator::handleStoreMiss(uint32_t memoryAddress){
    // What to do if it misses
    if(writeAllocate){
        if (writeThrough){
            load(memoryAddress, false);
            totCycles += 100; // handleCycles
        } else {
            load(memoryAddress, true);
        }
    } else {
        totCycles+=100; // handleCycles
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