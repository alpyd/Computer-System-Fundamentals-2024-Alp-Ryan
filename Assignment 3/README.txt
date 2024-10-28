Alp Demirtas (ademirt1)
Ryan Amer (ramer2)

Report: Use gcc.trace and swim.trace, fully associative caches of large size are not possible given hardware
Test a space of comparable cache configurations (same capacity) on realistic traces, find the realistic (not fully associative) config that performs best

***ALP TODO:
All of the data to use in our report is below, we can compare at hit rate, miss penalty, and total cycle in the report.
Look into determining the cache overhead for the write policies, LRU/FIFO to include in report
Here's a review of what each metric is:

Factors to look at:
Hit Rate: Higher hit rates usually indicate better use of locality.
Miss Penalty: Calculated based on the cycles spent on memory accesses due to cache misses.
Total Cycles (Execution Time): This is a practical measure of efficiency for each configuration.
Cache Overhead: Include the cost of extra bits (e.g., tags, valid, and dirty bits) required for different associativities and write policies

Setup: Testing gcc.trace and swim.trace files
Make sure to use the same cache overall capacity

Compare Associativity: Same Cache Size (8192 bytes), Different Associativity

./csim 256 2 16 write-allocate write-back fifo < gcc.trace (2-Way)
Total loads: 318197
Total stores: 197486
Load hits: 312298
Load misses: 5899
Store hits: 187483
Store misses: 10003
Total cycles: 11367381

Hit Rate: 97%

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

Hit Rate: 97%
Miss Penalty: Minor decreasing with increasing associativity
Pretty much no significant differences in all the statistics


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
