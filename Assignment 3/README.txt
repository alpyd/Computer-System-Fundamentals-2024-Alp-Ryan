Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

Report: Use gcc.trace and swim.trace, fully associative caches of large size are not possible given hardware
Test a space of comparable cache configurations (same capacity) on realistic traces, find the realistic (not fully associative) config that performs best

Setup: Testing gcc.trace and swim.trace files
Make sure to use the same block size and number of sets

Compare Associativity: Same Cache Size (8192 bytes), Different Associativity

./csim 256 2 16 write-allocate write-back fifo < gcc.trace (2-Way)
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

./csim 32 16 16 write-allocate write-back fifo < gcc.trace (16-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313151
Load misses: 5046
Store hits: 187761
Store misses: 9725
Total cycles: 10758512

./csim 8 64 16 write-allocate write-back fifo < gcc.trace (64-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313243
Load misses: 4954
Store hits: 187755
Store misses: 9731
Total cycles: 10712998

./csim 4 128 16 write-allocate write-back fifo < gcc.trace (128-Way)
Total loads: 318197
Total stores: 197486
Load hits: 313288
Load misses: 4909
Store hits: 187749
Store misses: 9737
Total cycles: 10693837


Compare LRU v FIFO: (Compare the two for a couple different cache sizes)





Compare Write Thru v Write Back: 





Compare Write Allocate v No Write Allocate


Factors to look at:
Hit Rate: Higher hit rates usually indicate better use of locality.
Miss Penalty: Calculated based on the cycles spent on memory accesses due to cache misses.
Total Cycles (Execution Time): This is a practical measure of efficiency for each configuration.
Cache Overhead: Include the cost of extra bits (e.g., tags, valid, and dirty bits) required for different associativities and write policies