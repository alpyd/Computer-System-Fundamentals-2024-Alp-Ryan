Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

Report: Use gcc.trace and swim.trace, fully associative caches of large size are not possible given hardware
Test a space of comparable cache configurations (same capacity) on realistic traces, find the realistic (not fully associative) config that performs best

Experiments Ran:
We first looked into the effect of associativity on cache performance. While maintaining the same overall cache capacity of 8192 bytes, same cache hit/miss policies, and same eviction policy, we varied the associativity ranging from 2-way to 128-way to isolate associativity.
We then looked into the effect of write allocate v no write allocate and write thru v write back by running all of the combinations on caches with same associativity, size, and eviction policy, to isolate solely store hit/miss policy impacting cache performance metrics.
We then looked into the effect of eviction policy, LRU v FIFO, by testing cache configurations identical in every aspect besides eviction policy, to again exclude other impacting variables and isolate eviction policy's effect on performance. 

We found the following:

Associativity Tests:
Hit Rate: 97% among all associativities
Miss Penalty: 16-Way Associativity and larger sees 22% less load misses compared to 2-Way Associativity
Pretty much no significant differences in all other statistics

Store Hit/Miss Policy Tests:
Write allocate leads to much less load and store misses than no write allocate
Write back uses less total cycles by about a factor of 10 than write through
Write allocate and write back seems to be the best combination with least cache misses and cycles

LRU V FIFO Tests:
Across the board, more total cycles in FIFO by roughly 1 million!
20% less load misses in LRU!


In our opinion, the most efficient cache configuration uses a higher associativity with write allocate, write back, and LRU eviction.
It balances the most common needs in cache performance by managing writes effectively, minimizing conflict misses, and retaining frequently accessed data through LRU.

Higher associativity (e.g., 16-way) reduces the likelihood of conflict misses (when multiple memory addresses map to the same cache set), as more options are available for where to store each block within a set.
Write allocate brings data into the cache on a write miss, enabling future accesses to benefit from faster cache access rather than having to go to main memory.
Write back reduces costly memory transcations, as multiple writes to the same cache block only trigger a single write-back when the data is eventually evicted. This is more efficient for applications with frequent updates to the same memory locations.
LRU eviction ensures that the least recently accessed data is evicted first, which is beneficial in accessing data with temporal locality (data accessed recently is likely to be accessed again soon).

Depending on the situation, some configurations are better suited than others. For example, if you have mostly sequential data access, a lower associativity (like 4-way or 8-way) with a FIFO eviction policy would be more efficient as it suited for predictable access patterns.
Write through cache configurations may be more efficient for data accessed only once

***ALP TODO:
Consider cache overhead()
What do u think about my idea for most efficient cache


Factors to look at:
Hit Rate: Higher hit rates usually indicate better use of locality.
Miss Penalty: Calculated based on the cycles spent on memory accesses due to cache misses.
Total Cycles (Execution Time): This is a practical measure of efficiency for each configuration.
Cache Overhead: Include the cost of extra bits (e.g., tags, valid, and dirty bits) required for different associativities and write policies

Setup: Testing gcc.trace and swim.trace files
Make sure to use the same cache overall capacity

Compare Associativity: Same Cache Size (8192 bytes), Different Associativity

./csim 256 2 16 write-allocate write-back lru < gcc.trace (2-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313059
Load misses: 5138
Store hits: 187779
Store misses: 9707
Total cycles: 10704438

./csim 128 4 16 write-allocate write-back lru < gcc.trace (4-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314007
Load misses: 4190
Store hits: 188015
Store misses: 9471
Total cycles: 10058022

./csim 32 16 16 write-allocate write-back lru < gcc.trace (16-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314233
Load misses: 3964
Store hits: 188076
Store misses: 9410
Total cycles: 9901909

./csim 8 64 16 write-allocate write-back fifo < gcc.trace (64-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314269
Load misses: 3928
Store hits: 188092
Store misses: 9394
Total cycles: 9875561

./csim 4 128 16 write-allocate write-back fifo < gcc.trace (128-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 9867174

Hit Rate: 97%
Miss Penalty: Minor decreasing with increasing associativity
Pretty much no significant differences in all the statistics

Compare Write Thru/Write Back and Write Allocate/No-Write-Allocate (Using same cache size, LRU):

./csim 4 128 16 write-allocate write-back lru < gcc.trace (Write Allocate, Write Back)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 9867174

./csim 4 128 16 write-allocate write-through lru < gcc.trace (Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 24819674

./csim 4 128 16 no-write-allocate write-through lru < gcc.trace (No Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 311234
Load misses: 6963
Store hits: 164433
Store misses: 33053
Total cycles: 23009467

Initial Thoughts: 
Write allocate leads to much less load and store misses than no write allocate
Write back uses less total cycles by about a factor of 10 than write through
Write allocate and write back seems to be the best combination with least misses and cycles


Compare LRU v FIFO: (Compare the two for a couple different cache sizes)

./csim 256 2 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 313059
Load misses: 5138
Store hits: 187779
Store misses: 9707
Total cycles: 10704438

./csim 256 2 16 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 312298
Load misses: 5899
Store hits: 187483
Store misses: 10003
Total cycles: 11367381

./csim 4 128 16 write-allocate write-back lru < gcc.trace (128-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 9867174

./csim 4 128 16 write-allocate write-back fifo < gcc.trace (128-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313288
Load misses: 4909
Store hits: 187749
Store misses: 9737
Total cycles: 10693837

More total cycles in FIFO by roughly 1 million!
20% less load misses in LRU!


ALP COMMANDS:

./csim 32 16 16 write-allocate write-back lru < gcc.trace (16-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314233
Load misses: 3964
Store hits: 188076
Store misses: 9410
Total cycles: 9901909

./csim 4 128 16 write-allocate write-back fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 313288
Load misses: 4909
Store hits: 187749
Store misses: 9737
Total cycles: 10693837

./csim 4 128 16 write-allocate write-through fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 313288
Load misses: 4909
Store hits: 187749
Store misses: 9737
Total cycles: 25317937

./csim 4 128 16 no-write-allocate write-through fifo < gcc.trace 
Total loads: 318197
Total stores: 197486
Load hits: 310232
Load misses: 7965
Store hits: 162297
Store misses: 35189
Total cycles: 23407129
