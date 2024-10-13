#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <vector>

struct Slot {
    uint32_t tag;
    bool valid, dirty;
    uint32_t load_ts,
    access_ts;
};
struct Set {
    std::vector<Slot> slots;
};
struct Cache {
    std::vector<Set> sets;
};

#endif

