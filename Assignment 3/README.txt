Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

Introduction: 
Experimental testing on different parameters of a cache with 8192 bytes and a 16-byte block size revealed that the cache configuration with the best overall effectiveness was a cache with 8-way set associativity, a write-allocate miss policy, a write-back hit policy, and a least recently used eviction policy.
The performance of each cache configuration was determined by examining the cache's total cycles, a practical measure of the efficiency of each operation, and the cache's hit rate, an indicator of better use of locality. 
Additionally, the feasibility of the cache configuration was taken into account significantly when deciding the set associativity, as engineers would likely not want to incur the additional complexity and cost of adding more associativity if there was no real benefit.
The miss penalty and the number of extra bits required for the different configurations, such as tracking tags, validity, and dirty bits, were also considered when assessing performance, but they were less important in the final selection of parameters because the miss penalty would be factored into the efficiency of the total cycles, and utilizing extra bits could be helpful if it leads to better efficiency.

Methods: 
Each combination of parameters was run using the inputs provided in gcc.trace, which simulated a realistic series of load and store commands that a cache may experience. 
First, the effect of associativity on cache performance was isolated and tested. While maintaining the same overall cache capacity of 8192 bytes, same cache hit/miss policies, and same eviction policy, the associativity was varied, ranging from 2-way to 128-way.
Finally, the effect of the eviction policy, LRU v FIFO, was isolated and tested by observing how the total cycles and hit rate changed with each eviction policy. 
Finally, the effects of a write-allocate vs. no-write-allocate miss policy and write-through vs. write-back hit policy were isolated and tested by running all of the combinations on caches with the same associativity, size, and eviction policy.

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


The first set of parameters tested was the number of sets and blocks, which would determine the optimal associativity.
For all of the runs, the cache's total size was 8192 bytes, and each block was 16 bytes, which meant that 512 blocks would need to be evenly distributed.
The tested [set, block] pairs were [2:256], [4:128], [8:64], [16:32], [32:16], [64:8], [128:4], and [256:2] under a FIFO eviction policy, write-allocate miss policy, and write-back hit policy.

While the results shown above demonstrate that the 128-way set associativity had the lowest total cycles at 10693837 and the highest hit rate of 97.16%, there was not much of a difference between the associativities. The largest difference in hit rate was 0.24%, and the largest difference in cycles was that the 128-way set associativity used 6% fewer total cycles than the 2-way set associativity.
Therefore, since increasing associativity did not significantly affect the total cycles and hit rate, the most optimal associativity was chosen based on observing when the returns were 80% of the tested maximum.
The total cycles decreased by 4.23% when increasing associativity from 2-way to 4-way, 0.87% when increasing associativity from 4-way to 8-way, 0.31% when increasing associativity from 8-way to 16-way, and 0.18% when increasing associativity from 16-way to 32-way.

As demonstrated by these decreases, there were increasingly diminishing returns when increasing associativity. 
The total cycles decrease from 2-way to 8-way was 5.06%, whereas the total cyles decrease from 8-way to 128-way was 0.91%, which meant that the total cycles at 8-way associativity were ~84.82% of the 128-way associativity.
This is further corroborated by the hit rate, which increased 0.2% from 2-way to 8-way associativity but only by 0.04% from 8-way to 128-way associativity. This indicates that the hit rate at 8-way associativity was around ~83.33% of the 128-way associativity.

Therefore, the 8-way associativity was chosen as the "optimal" associativity because it represented around 84% of the maximum observed performance while having 16 times less complexity than the 128-way associativity.

Eviction Policy Test Results and Discussion:

./csim 64 8 16 write-allocate write-back lru < gcc.trace (8-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 9913888
Hit Rate: 97.404%

./csim 32 16 16 write-allocate write-back lru < gcc.trace (16-Way)
Total loads: 318197
Total stores: 197486
Load hits: 314233
Load misses: 3964
Store hits: 188076
Store misses: 9410
Total cycles: 9901909
Hit Rate: 97.406%

./csim 4 128 16 write-allocate write-back lru < gcc.trace (128-way)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 9867174
Hit Rate: 97.419%

The second set of tested parameters was whether the eviction policy should be FIFO, first-in, first-out, or LRU, least recently used. 
The differences across set associativities were even smaller for LRU than FIFO, with the decrease in total cycles being 0.12% from 8-way to 16-way and 0.35% from 16-way to 128-way. The hit rate only decreased by 0.015% between the max and min hit rates.
However, the hit rate using LRU or FIFO for the same set-associativity differed consistently and significantly.

When comparing the set associativities with FIFO versus LRU, LRU 8-way associativity saw a hit rate increase of 0.28% and total cycles decrease of 8.13%, LRU 16-way associativity saw a hit rate increase of 0.27% and total cycles decrease of 7.96%, and LRU 128-way associativity saw a hit rate increase of 0.26% and total cycles decrease of 7.73%.
This ~7-8% decrease in total cycles and around ~0.25% increase in hit rate demonstrates that an LRU eviction policy performs much better than a FIFO eviction policy.
Even though LRU requires more operations, as the access timestamp needs to be continually updated, this overhead of operations is worth it because it leads to significantly fewer cycles.
LRU eviction ensures that the least recently accessed data is evicted first, which is beneficial in accessing data with temporal locality (data accessed recently is likely to be accessed again soon).
FIFO may be better than LRU, depending on the situation. For example, FIFO would be better for sequential data access, as the first data accessed would be the first out after a certain amount of time.


Hit/Miss Policy Test Results and Discussion:

./csim 64 8 16 write-allocate write-back lru < gcc.trace (8-Way Associativity, LRU Eviction - Write Allocate, Write Back)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 9913888

./csim 64 8 16 write-allocate write-through lru < gcc.trace (8-Way Associativity, LRU Eviction - Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 314221
Load misses: 3976
Store hits: 188067
Store misses: 9419
Total cycles: 24856588

./csim 64 8 16 no-write-allocate write-through lru < gcc.trace (8-Way Associativity, LRU Eviction - No Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 311155
Load misses: 7042
Store hits: 164406
Store misses: 33080
Total cycles: 23040961

./csim 4 128 16 write-allocate write-back lru < gcc.trace (128-Way Associativity, LRU Eviction - Write Allocate, Write Back)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 9867174

./csim 4 128 16 write-allocate write-through lru < gcc.trace (128-Way Associativity, LRU Eviction - Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 314281
Load misses: 3916
Store hits: 188093
Store misses: 9393
Total cycles: 24819674

./csim 4 128 16 no-write-allocate write-through lru < gcc.trace (128-Way Associativity, LRU Eviction - No Write Allocate, Write Through)
Total loads: 318197
Total stores: 197486
Load hits: 311234
Load misses: 6963
Store hits: 164433
Store misses: 33053
Total cycles: 23009467

Since the miss policy could either be write-allocate or no-write-allocate and the hit policy could either be write-through or write-back, combinations of these hit/miss policies were tested, as some hit-miss strategies may be more synergistic than others.
The hit/miss policy of write-back and no-write-allocate was not tested, as it would render the cache useless. 

When the remaining 3 hit/miss policy combinations were tested for the 8-way set associativity with LRU eviction, write-allocate and write-back had 9913888 total cycles, write-allocate and write-through had 24856588 total cycles, and no-write-allocate and write-through had 23040961 total cycles.
As demonstrated by these total cycles, write-back performed better than write-through with a ~57% - ~60% decrease in the total cycles.
To find the second best combination, while no-write-allocate performed 7.72% better than write-allocate for the write-through hit policy, this difference also led to around 2 times more load misses and 4 times more store misses.
While no-write-allocate and write-through performed better for this set of commands, it should be noted that commands that lead to more misses could cause it to be less effective.

The 128-way set associativity with LRU eviction supported the findings that write-allocate and write-back were optimal, as the write-allocate and write-back had 9867174 total cycles, write-allocate and write-through had 24819674 total cycles, and no-write-allocate and write-through had 23009467 total cycles.
In this case, write-back consistently outperformed write-through, with a similar ~57%—60% decrease in the total cycles.
Even though write-back does have additional overhead to store the dirty bit, the increased efficiency of greater than 50% makes the extra bit worth it.

Writing through cache configurations may be more efficient for other commands where the data is only accessed once.
However, these are specific cases and are not representative of the broad spectrum of load and store actions that will be conducted.

Therefore, the optimal hit-miss policy combination was write-allocate and write-back because it had 60% fewer total cycles.
Write-allocate brings data into the cache on a miss, enabling future accesses to benefit from faster cache access rather than going to main memory.
Write-back reduces costly memory transcations, as multiple writes to the same cache block only trigger a single write-back when the data is eventually evicted, which is more efficient for applications with frequent updates to the same memory locations.


Conclusion:

As demonstrated by the results from testing the different cache parameter configurations, the optimal configuration was a cache with 8-way set associativity, LRU eviction, a write-allocate miss policy, and a write-back hit policy.
It balances the most common needs in cache performance by managing writes effectively, minimizing conflict misses, and retaining frequently accessed data through LRU.
The 8-way set associativity was chosen because it was a set associativity that had greater than ~80% of the returns in the highest tested set associativity and could still be realistically implemented in the real world. After all, higher associativity caches would require more money and complexity.
The LRU eviction policy and write-back, write-allocate hit-miss policy outperformed their counterparts, leading to a significantly lower number of cycles, a primary indicator for performance.
While there exist certain cases where the FIFO and write-through policies could outperform the chosen parameters, these parameters require specific cases. They are not as general enough to be used in an all-purpose computer, as demonstrated by the commands in gcc.trace.


Contributions:
Both partners contributed synergistically towards the project
Alp wrote the implementations of the cache simulator and its main functions, as well as the report for the optimal cache configuration.
Ryan tested and debugged the implementations of the cache simulator and main functions, ensuring that the total cycles fell within the desired range. He also gathered all the report data by running the shown commands on his computer.