Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

From experimental testing on different parameters of a cache with 8192 bytes and 16 byte block size, the optimal determined combination was a cache with 4 sets and 128 blocks in each set with a write-allocate miss policy, write-back hit policy, and a least recently used eviction policy.
Since these parameters are trying to test realistic parameters, fully-associative configurations were not tested.
Each combination of parameters was run using the inputs provided in gcc.trace, which simulated a series of load and store commands. 
The performance of each cache configuration was determined by looking at the cache's hit rate, an indocator for better use of locality, and total cycles, a practical measure of the efficiency of each operation.
The miss peanlty and the number of extra bits required for the different configurations, such as tracking tags, validity, and dirty bits, were also considered when assessing performance, but they beared less importance on the final selection of parameters because the miss penalty would be factored into the efficiency of the total cycles and there were no constraints on using extra bits.

The first set of parameters that was tested were the number of sets and blocks, which would determine the optimal associativity.
Since there were 512 blocks and it would not be realistic to have a fully-associative configuration, the tested set:block pairs were [2:256], [4:128], [8:64], [16:32], [32:16], [64:8], [128:4], and [256:2].
As demonstrated in the results shown below, the 128-way pair of 128 sets and 4 blocks performed the best, as it had the lowest cycles at 10693837 and highest hit rate at 97% when all pairs were tested with the FIFO, write-allocate, and write-back parameters.
Comparatively, the 2-way 

Compare Associativity: Same Cache Size (8192 bytes), Different Associativity

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

./csim 256 2 16 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 313059
Load misses: 5138
Store hits: 187779
Store misses: 9707
Total cycles: 10704438

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
