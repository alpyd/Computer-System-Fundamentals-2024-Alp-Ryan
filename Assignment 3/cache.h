#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <vector>
// Provided struct for Slot in class
struct Slot {
    uint32_t tag;
    bool valid, dirty;
    uint32_t load_ts,
    access_ts;
};

// Set is represented as a vector of Slots
struct Set {
    std::vector<Slot> slots;
};

// Cache is represented as a vector of Sets
struct Cache {
    std::vector<Set> sets;
};

#endif

