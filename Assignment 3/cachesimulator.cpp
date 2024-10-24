#include "cache.h"
#include "cachesimulator.h"
#include <iostream>
#include <cmath>

// Constructor for the CacheSimulator takes in the user-inputted parameters and initializes the tracker variables to 0
CacheSimulator::CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU)
    : numOfSets(numOfSets), setSize(setSize), blockSize(blockSize), writeAllocate(writeAllocate), writeThrough(writeThrough), evictionLRU(evictionLRU),
    totLoads(0), totStores(0), totCycles(0), LTotHits(0), LTotMisses(0), STotHits(0), STotMisses(0), timestamp(0)  {

    // Populate an empty cache
    for(int i = 0; i < numOfSets; i++){
        // Create a new Set based on the number of user-specified Sets
        Set newSet;
        for (int j = 0; j < setSize; j++){
            // Create a new Slot based on the number of user-specified Slots
            Slot newSlot;

            // Set the valid and dirty parameters to false and add the newSlot
            newSlot.valid = false;
            newSlot.dirty = false;
            newSet.slots.push_back(newSlot);
        }
        cache.sets.push_back(newSet);
    }
}


void CacheSimulator::executeCommand(char command, uint32_t memoryAddress){
    // If the command is 's' for store, call a store
    if(command == 's') {
        // Increase the total store calls
        totStores++;

        //Track number of store hits/misses and call store method
        if(store(memoryAddress)){ 
            STotHits++;
        } else {
            STotMisses++;
        }
    } else if(command == 'l'){ 
        // If the command is 'l' for load, call a load and increase the total load calls
        totLoads++;

        // Track of load hits/misses
        if(load(memoryAddress, false)){
            LTotHits++;
        } else {
            LTotMisses++;
        }
    } 
}

uint32_t CacheSimulator::readTag(uint32_t memoryAddress){
    // Take the natural log of the offset and index sizes to get the number of bits
    int numBitsOffset = std::log(blockSize)/std::log(2);
    int numBitsIndex = std::log(numOfSets)/std::log(2);

    // Right shift the memory address by the number of total offset and index bits to get the tap
    return memoryAddress >> (numBitsOffset + numBitsIndex);
}

uint32_t CacheSimulator::readIndex(uint32_t memoryAddress){
    // Take the natural log of the offset and index sizes to get the number of bits
    int numBitsOffset = std::log(blockSize)/std::log(2);
    int numBitsIndex = std::log(numOfSets)/std::log(2);

    // Right shift by the number of offset bits and then isolate the number of index bits using &
    return (memoryAddress >> numBitsOffset) & ((1 << numBitsIndex) - 1);
}

bool CacheSimulator::load(uint32_t memoryAddress, bool isDirty){
    // Read the tag and index from the memory index
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readTag(memoryAddress);

    // Locate the desired set from the index
    Set &set = cache.sets[index];
    
    // Iterate over the Slots in a set
    for (unsigned int i = 0; i < set.slots.size(); i++){ 
        // Get the slot at a specific index
        Slot &slot = set.slots.at(i);

        // If the solid is found to match, success! Load.
        if(slot.valid && slot.tag == tag){    
            // Set the access timestamp to current timestamp 
            slot.access_ts = ++timestamp;

            // Carry the data if the slot is dirty
            if(isDirty){
                slot.dirty = true;
            }

            // 1 cycle per load operation
            totCycles++; 
            return true;
        }
    }

    // If could not hit the load, handle the load miss
    handleLoadMiss(set, tag, isDirty);

    // Return false, indicating that there was a miss
    return false;
    

}

void CacheSimulator::handleLoadMiss(Set& set, uint32_t tag, bool isDirty){
    // Choose the slot to load into
    Slot chosenSlot = set.slots.at(chooseSlotIndex(set));

    // If FIFO policy, update the timestamp
    if(!evictionLRU){
        chosenSlot.load_ts = ++timestamp;
    }

    if(chosenSlot.valid && chosenSlot.dirty){ 
        // Add 100 cycles per 4 byte quantity
        totCycles += ((blockSize / 4)  * 100);
    }

    // Update the Slot with the provided parameters from the execution and memory Address
    chosenSlot.tag = tag;
    chosenSlot.valid = true;
    chosenSlot.dirty = isDirty;
    chosenSlot.access_ts = ++timestamp;

    // Add 100 cycles per 4 byte quantity
    totCycles += ((blockSize / 4) * 100); 
}

uint32_t CacheSimulator::chooseSlotIndex(Set& set){
    // First see if there are any invalid or uninitialized slots
    // Return that slot index if it is
    for(unsigned int i = 0; i < set.slots.size(); i++){
        if(!set.slots.at(i).valid){
            return i;
        }
    }

    // Otherwise, use the eviction policy to select the slot to be evicted
    int chosenIndex = 0;
    if(evictionLRU) {
        // If eviction is LRU, choose the slot that was accessed least recently (linear search)
        for(unsigned int i = 0; i < set.slots.size(); i++){
            if(set.slots.at(i).access_ts < set.slots.at(chosenIndex).access_ts){
                chosenIndex = i;
            }
        }
    } else {
        // If eviction is FIFO, choose the slot that was loaded least recently (linear search)
        for(unsigned int i = 0; i < set.slots.size(); i++){
            if(set.slots.at(i).load_ts < set.slots.at(chosenIndex).load_ts) {
                chosenIndex = i;
            }
        }
    }

    // Return the chosen index once found under these parameters
    return chosenIndex;
}

bool CacheSimulator::store(uint32_t memoryAddress){
    // Read tag and index from memory address
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readTag(memoryAddress);

    // Locate the desired Set using the provided index
    Set &set = cache.sets[index];

    for (unsigned int i = 0; i < set.slots.size(); i++){
        // Specify a slot of a certain index in the Set
        Slot slot = set.slots.at(i);
        if(slot.valid && slot.tag == tag){
            // Success! We found tag, let's store now.
            timestamp++;

            // Update access timestamp for LRU eviction
            slot.access_ts = timestamp;

            // Handle write-through and write-back instances
            if(writeThrough){
                load(memoryAddress, false);
                // Handle cycles
                totCycles += 100; 
            } else {
                load(memoryAddress, true);
            }

            // Increment cycles and return true for successful store
            totCycles++;
            return true;
        }
    }

    // If not able to find the tag, handle the store miss
    handleStoreMiss(memoryAddress);
    return false;

}

void CacheSimulator::handleStoreMiss(uint32_t memoryAddress){
    // What to do if it misses for the diffent cases of write-allocate or no-write-allocate and write-through or write-back
    if(writeAllocate){
        // Handle either write through or write back case
        if (writeThrough){
            load(memoryAddress, false);

            // handleCycles
            totCycles += 100; 
        } else {
            load(memoryAddress, true);
        }
    } else {
        // handleCycles
        totCycles+=100; 
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