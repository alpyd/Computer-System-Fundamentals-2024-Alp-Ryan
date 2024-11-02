#include "cache.h"
#include "cachesimulator.h"
#include <iostream>
#include <cmath>

/* Constructor for CacheSimulator that sets all counters to 0 and takes in user parameters

Parameters:
   numOfSets - number of sets in cache
   setSize - how many slots are in a set
   blockSize - how many bytes is the size of each block
   writeAllocate - whether miss policy is write-allocate or no-write allocate
   writeThrough - whether hit policy is write-through or write-back
   evictionLRU - whether eviction policy is LRU or FIFO
*/  

CacheSimulator::CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU)
    : numOfSets(numOfSets), setSize(setSize), blockSize(blockSize), 
      writeAllocate(writeAllocate), writeThrough(writeThrough), evictionLRU(evictionLRU), 
      totCycles(0), STotMisses(0), timestamp(0) {

    // Initialize the cache by initializing each of the sets and initializing their respective nodes
    for(int i = 0; i < numOfSets; i++){
        // Initialize the set at that index and then add all of the slots as specified by parameters
        Set newSet;
        for (int j = 0; j < setSize; j++){
            Slot newSlot;
            
            // Slot is neither dirty nor valid when first initialized
            newSlot.valid = false;
            newSlot.dirty = false;

            // Initialize access_ts
            newSlot.access_ts = 0;

            // Initialize load_ts 
            newSlot.load_ts = 0;   

            // Add the slot into the set
            newSet.slots.push_back(newSlot);
        }

        //Add the set into the cache
        cache.sets.push_back(newSet);
    }
}


/* Execute a load or store command

Parameters:
   command - character value indicating load or store
   memoryAddress - memory address with which to simulate load or store
*/

void CacheSimulator::executeCommand(char command, uint32_t memoryAddress){
    if(command == 's') {
        // Increase the total store commands
        totStores++;
        if(store(memoryAddress)){ 
            // Increase hits if store returns true
            STotHits++;
        } else {
            // Increment misses if store returns false
            STotMisses++;
        }
    } else if(command == 'l'){ 
        // Increase the total load commands
        totLoads++;
        if(load(memoryAddress, false)){
            // Increase hits if load returns true
            LTotHits++;
        } else {
            // Increase misses if load returns false
            LTotMisses++;
        }
    } 
}

/* Read the tag from a provided memory address

Parameters:
   memoryAddress - the memory address that will be read
*/  

uint32_t CacheSimulator::readTag(uint32_t memoryAddress) {
    // Number of bits for the block offset
    int numBitsOffset = std::log2(blockSize); 

    // Number of bits for the index
    int numBitsIndex = std::log2(numOfSets);   

    // Return the tag by shifting by the sum of offset and index bits
    return memoryAddress >> (numBitsOffset + numBitsIndex);
}

/* Read the index from a provided memory address

Parameters:
   memoryAddress - the memory address that will be read
*/  

uint32_t CacheSimulator::readIndex(uint32_t memoryAddress) {
    // Number of bits for the block offset
    int numBitsOffset = std::log2(blockSize);  

    // Number of bits for the index
    int numBitsIndex = std::log2(numOfSets);   

    // Return the index by shifting first by the number of offset bits and then masking the index bits
    return (memoryAddress >> numBitsOffset) & ((1 << numBitsIndex) - 1);
}

/* Chooses a slot index to load a memory address into on a miss

Parameters:
    set - the provided set to choose the dedicated slot from
*/  


uint32_t CacheSimulator::chooseSlotIndex(Set& set){
    // Conduct a linear search to find the first slot that is invalid; holds nothing
    for(uint32_t i = 0; i < set.slots.size(); i++){
        if(!set.slots.at(i).valid){
            return i;
        }
    }

    // If all slots are valid, then we must evict using either LRU or FIFO
    int chosenIndex = 0;
    if(evictionLRU) {
        for(uint32_t i = 0; i < set.slots.size(); i++){
            // Set the chosen index to the slot that was accessed last for LRU
            if(set.slots.at(i).access_ts < set.slots.at(chosenIndex).access_ts){
                chosenIndex = i;
            }
        }
    } else {
        for(uint32_t i = 0; i < set.slots.size(); i++){
            // Set the chosen index to the slot that was loaded in last
            if(set.slots.at(i).load_ts < set.slots.at(chosenIndex).load_ts) {
                chosenIndex = i;
            }
        }
    }

    // Return the index that was going to be evicted
    return chosenIndex;
}

/* Loads a memory address into the cache

Parameters:
   memoryAddress - the memory address to load into the cache
   isDirty - whether the slot being loaded is dirty
*/  

bool CacheSimulator::load(uint32_t memoryAddress, bool isDirty){ 
    // Isolate the tag and index of the memory address
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readIndex(memoryAddress);

    // Locate the set using the provided index
    Set &set = cache.sets[index];

    // Iterate over slots within the sets to find the tag
    for (unsigned int i = 0; i < set.slots.size(); i++){ 
        Slot &slot = set.slots.at(i);
        if(slot.valid && slot.tag == tag){    
            // If there is a matching tag, return true and increase access timestamp for LRU
            slot.access_ts = ++timestamp;
            if(isDirty){
                slot.dirty = true;
            }
            //1 cycle per load operation
            totCycles++; 
            return true;
        }
    }
    
    // If the tag was not found, we must execute a load miss
    handleLoadMiss(set, tag, isDirty);
    return false;
    
}

/* Handle a load miss

Parameters:
   set - The set with which to handle the load miss in
   tag - The tag of the memory address being loaded in
   isDirty - Whether or not the slot being loaded into is dirty
*/  

void CacheSimulator::handleLoadMiss(Set& set, uint32_t tag, bool isDirty){ 
    // Get the chosen Slot to write into
    Slot &chosenSlot = set.slots.at(chooseSlotIndex(set));

    // For FIFO, make sure to update load timestamp
    if(!evictionLRU){
        chosenSlot.load_ts = ++timestamp;
    }


    if(chosenSlot.valid && chosenSlot.dirty){ 
        // Add 100 cycles per 4 byte quantity
        totCycles += ((blockSize / 4)  * 100);
    }

    // Load in the given tag and its properties
    chosenSlot.tag = tag;
    chosenSlot.valid = true;
    chosenSlot.dirty = isDirty;

    // Increase the access timestamp for LRU
    chosenSlot.access_ts = ++timestamp;

    // Add 100 cycles per 4 byte quantity
    totCycles += ((blockSize / 4) * 100); 
}

/* Stores a memory address from the cache

Parameters:
   memoryAddress - the memory address to store into the cache
*/  

bool CacheSimulator::store(uint32_t memoryAddress){ 
    uint32_t tag = readTag(memoryAddress);
    uint32_t index = readIndex(memoryAddress);
    Set &set = cache.sets[index];
    
    // Check if the tag is already in the cache (cache hit)
    for (unsigned int i = 0; i < set.slots.size(); i++){
        // Use a reference to modify the slot directly
        Slot &slot = set.slots.at(i);  
        if(slot.valid && slot.tag == tag){
            timestamp++;
            slot.access_ts = timestamp;

            // Perform the appropriate action based on writeThrough parameter
            if (writeThrough) {
                // Handle cycles for write through
                totCycles += 100; 
                slot.dirty = false;
            } else {
                // Mark the slot as dirty since we're storing data
                slot.dirty = true; 
            } 
            // Increment total cycles for the operation and return true indicating a hit
            totCycles++; 
            return true;
        }
    }
    // Handle a store miss
    handleStoreMiss(memoryAddress); 

    // Return false if the store was unsuccessful
    return false; 
}

/* Handle a store miss

Parameters:
   memoryAddress - the memory address that will be stored on the miss
*/  

void CacheSimulator::handleStoreMiss(uint32_t memoryAddress){
    // On a store miss, load the relevant block into the cache if write-allocate
    if (writeAllocate) {
        // Load and mark as dirty
        load(memoryAddress, true); 
    } else {
        // No-write-allocate: Write directly to memory without cache modification
        // Writing to memory directly (100 cycles)
        totCycles += 100; 
    }
}

/* Print the summary info with the total loads, stores, and cycles, 
   as well as the load and store hits and misses
*/  

void CacheSimulator::printSummaryInfo(){
    // Print out the total loads and stores
    std::cout << "Total loads: " << totLoads << std::endl;
    std::cout << "Total stores: " << totStores << std::endl;
    
    // Print out the load hits and misses
    std::cout << "Load hits: " << LTotHits << std::endl;
    std::cout << "Load misses: " << LTotMisses << std::endl;
    std::cout << "Store hits: " << STotHits << std::endl;
    std::cout << "Store misses: " << STotMisses << std::endl;

    // Print out the total cycles
    std::cout << "Total cycles: " << totCycles << std::endl;
}