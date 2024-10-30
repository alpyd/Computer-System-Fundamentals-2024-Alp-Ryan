Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

Introduction: 
From experimental testing on different parameters of a cache with 8192 bytes and 16 byte block size, the optimal determined combination was a cache with 8-way set associativity with a write-allocate miss policy, write-back hit policy, and a least recently used eviction policy.
The performance of each cache configuration was determined by looking at the cache's total cycles, a practical measure of the efficiency of each operation, and the cache's hit rate, an indicator for better use of locality. 
Additionally, the feasibility of the cache configuration was taken into account significantly when deciding the set associativity, as engineers would likely not want to incur the additional complexity and cost of adding more associativity if there was no real benefit.
The miss peanlty and the number of extra bits required for the different configurations, such as tracking tags, validity, and dirty bits, were also considered when assessing performance, but they beared less importance on the final selection of parameters because the miss penalty would be factored into the efficiency of the total cycles and there were no constraints on using extra bits.

Methods: 
Each combination of parameters was run using the inputs provided in gcc.trace, which simulated a realsitic series of load and store commands that a cache may experience. 
First, the effect of associativity on cache performance was isolated and tested. While maintaining the same overall cache capacity of 8192 bytes, same cache hit/miss policies, and same eviction policy, the associativity was varied, ranging from 2-way to 128-way.
Finally, the the effect of eviction policy, LRU v FIFO, was isolated and tested by observing how the total cycles and hit rate of the eviction policy changed. 
Finally, the effects of a write-allocate v no-write-allocate miss policy and write-through v write-back hit policy were isolated and tested by running all of the combinations on caches with same associativity, size, and eviction policy.

Associativity Tests Results and Discussion:

./csim 256 2 16 write-allocate write-back fifo < gcc.trace (2-Way)
Total loads: 318197
Total stores: 197486
Load hits: 312298
Load misses: 5899
Store hits: 187483
Store misses: 10003
Total cycles: 11367381
Hit Rate: 96.92%

./csim 128 4 16 write-allocate write-back fifo < gcc.trace (4-Way)
Total loads: 318197
Total stores: 197486
Load hits: 312969
Load misses: 5228
Store hits: 187684
Store misses: 9802
Total cycles: 10886653
Hit Rate: 97.09%

./csim 64 8 16 write-allocate write-back fifo < gcc.trace (8-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313096
Load misses: 5101
Store hits: 187747
Store misses: 9739
Total cycles: 10791643
Hit Rate: 97.12%

./csim 32 16 16 write-allocate write-back fifo < gcc.trace (16-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313151
Load misses: 5046
Store hits: 187761
Store misses: 9725
Total cycles: 10758512
Hit Rate: 97.14%

./csim 8 64 16 write-allocate write-back fifo < gcc.trace (64-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313243
Load misses: 4954
Store hits: 187755
Store misses: 9731
Total cycles: 10712998
Hit Rate: 97.15%

./csim 4 128 16 write-allocate write-back fifo < gcc.trace (128-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313288
Load misses: 4909
Store hits: 187749
Store misses: 9737
Total cycles: 10693837
Hit Rate: 97.16%


The first set of parameters that was tested were the number of sets and blocks, which would determine the optimal associativity.
For all of the runs, the total size of the cache was 8192 bytes and each block size was 16 bytes, which meant that 512 blocks would need to be evenly distributed.
The tested set:block pairs were [2:256], [4:128], [8:64], [16:32], [32:16], [64:8], [128:4], and [256:2] under a FIFO eviction policy, write-allocate miss policy, and write-back hit policy.

While the results shown above demonstrate that the 128-way set associativity had the lowest total cycles at 10693837 and the highest hit rate of 97.16%, there was not much of a difference between the associativities, with the largest difference in hit rate being 0.24% and the largest difference in cycles being that the 128-way set associativity used 6% less total cycles than the 2-way set associativity.
Therefore, since increasing associativity did not have a large affect on the total cycles and hit rate, the most optimal associativity was chosen based on observing when the returns were 80% of the tested maximum.
The total cycles decreased by 4.23% when increasing associativity from 2-way to 4-way, 0.87% when increasing associativity from 4-way to 8-way, 0.31% when increasing associativity from 8-way to 16-way, and 0.18% when increasing associativity from 16-way to 32-way.

As demonstrated by these decreases, there were increasingly diminishing returns when increasing associativity. 
The total cycles decrease from 2-way to 8-way was 5.06%, whereas the total cyles decrease from 8-way to 128-way was 0.91%, which meant that the total cycles at 8-way associativity were ~84.82% of the 128-way associativity.
This is further corroborated by the hit rate, as the hit rate increased 0.2% from 2-way to 8-way associativity but only increased by 0.04% from 8-way to 128-way associativity, indicating that the hit rate at 8-way associativity was around ~83.33% of the 128-way associativity.

Therefore, 8-way associativity was chosen as the "optimal" associativity because it represented around 84% of the maximum observed performance while having 16 times less complexity than the 128-way associativity.

Evicition Policy Test Results and Discussion:
Across the board, more total cycles in FIFO by roughly 1 million!
20% less load misses in LRU!


Hit/Miss Policy Test Results and Discussion:

Write allocate leads to much less load and store misses than no write allocate
Write back uses less total cycles by about a factor of 10 than write through
Write allocate and write back seems to be the best combination with least cache misses and cycles




In our opinion, the most efficient cache configuration uses a higher associativity with write allocate, write back, and LRU eviction.
It balances the most common needs in cache performance by managing writes effectively, minimizing conflict misses, and retaining frequently accessed data through LRU.

Higher associativity (e.g., 8/16-way) reduces the likelihood of conflict misses (when multiple memory addresses map to the same cache set), as more options are available for where to store each block within a set.
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


Compare Associativity: Same Cache Size (8192 bytes), Different Associativity

./csim 256 2 16 write-allocate write-back lru < gcc.trace (2-Way)
Total loads: 318197
Total stores: 197486
Load hits: 312298
Load misses: 5899
Store hits: 187483
Store misses: 10003
Total cycles: 11367381

./csim 128 4 16 write-allocate write-back fifo < gcc.trace (4-Way)
Total loads: 318197
Total stores: 197486
Load hits: 312969
Load misses: 5228
Store hits: 187684
Store misses: 9802
Total cycles: 10886653
Hit Rate: 97.09%

./csim 64 8 16 write-allocate write-back lru < gcc.trace (8-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 9913888

./csim 32 16 16 write-allocate write-back lru < gcc.trace (16-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313151
Load misses: 5046
Store hits: 187761
Store misses: 9725
Total cycles: 10758512


Hit Rate: 97.16%
Miss Penalty: Minor decreasing with increasing associativity
Pretty much no significant differences in all the statistics

Compare Write Thru/Write Back and Write Allocate/No-Write-Allocate (Using same cache size, LRU):

./csim 64 8 16 write-allocate write-back lru < gcc.trace (Write Allocate, Write Back)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 9913888

./csim 64 8 16 write-allocate write-through lru < gcc.trace (Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 24856588

./csim 64 8 16 no-write-allocate write-through lru < gcc.trace (No Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 311155
Load misses: 7042
Store hits: 164406
Store misses: 33080
Total cycles: 23040961

Initial Thoughts: 
Write allocate leads to much less load and store misses than no write allocate
Write back uses less total cycles by about a factor of 10 than write through
Write allocate and write back seems to be the best combination with least misses and cycles


Compare LRU v FIFO: (Compare the two for a couple different cache associativities)

./csim 256 2 16 write-allocate write-back lru < gcc.trace (2-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313059
Load misses: 5138
Store hits: 187779
Store misses: 9707
Total cycles: 10704438

./csim 256 2 16 write-allocate write-back fifo < gcc.trace (2-Way)
Total loads: 318197
Total stores: 197486
Load hits: 312298
Load misses: 5899
Store hits: 187483
Store misses: 10003
Total cycles: 11367381

./csim 64 8 16 write-allocate write-back lru < gcc.trace (8-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 9913888



More total cycles in FIFO by roughly 1 million!
20% less load misses in LRU!


ALP COMMANDS: **I don't think we need this anymore, let me know what u think? - Ryan

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
