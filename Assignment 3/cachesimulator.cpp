#include "cache.h"
#include "cachesimulator.h"
#include <iostream>

CacheSimulator::CacheSimulator(int numOfSets, int setSize, int blockSize, bool writeAllocate, bool writeThrough, bool evictionLRU){
    int totLoads = 0; 
    int totStores = 0; 
    int LTotHits = 0; 
    int LTotMisses = 0; 
    int STotHits = 0; 
    int STotMisses = 0; 
    int totCycles = 0; 
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

bool CacheSimulator::load(uint32_t memoryAddress, bool isDirty){

}

bool CacheSimulator::store(uint32_t memoryAddress){

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