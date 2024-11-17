CONTRIBUTIONS

Ryan wrote the implementation for Tasks 1, 2, and 3.
Alp debugged and verified Tasks 1, 2, and 3, conducted experiments for Task 4, and wrote the report.

REPORT

To ensure that the parasort program exhibits the expected degree of parallelism, multiple experiments were conducted with different forking thresholds of elements before calling qsort() (quicksort) using a random 16 MB file on the ugrad4 computer.
It was hypothesized that a lower threshold of elements before calling qsort() would lead to more fork() calls and more parallel processes, speeding up the computation time.

When the number of elements threshold was varied between 16384 (2^14) and 2097152 (2^11) on the ugrad4 computer, the following results were obtained:

Test run with threshold 2097152
real    0m0.384s
user    0m0.360s
sys     0m0.021s

Test run with threshold 1048576
real    0m0.216s
user    0m0.379s
sys     0m0.024s

Test run with threshold 524288
real    0m0.155s
user    0m0.415s
sys     0m0.039s

Test run with threshold 262144
real    0m0.123s
user    0m0.459s
sys     0m0.046s

Test run with threshold 131072
real    0m0.116s
user    0m0.444s
sys     0m0.063s

Test run with threshold 65536
real    0m0.108s
user    0m0.469s
sys     0m0.082s

Test run with threshold 32768
real    0m0.118s
user    0m0.488s
sys     0m0.110s

Test run with threshold 16384
real    0m0.123s
user    0m0.536s
sys     0m0.156s

The results indicate that the real time spent trying to execute the program generally decreased as the threshold number of elements decreased.
The real run time decreased from 0.384s to 0.216s when the threshold number decreased from 2097152 to 1048576, representing a shift from a single process to two parallel processes.
This 43.75% decrease in runtime demonstrates how even a shift from one single-process execution to two parallel processes leads to a significant increase in efficiency.
As the number of parallel processes increased, demonstrated by a continual decrease in the threshold number by a factor of 2, the most efficient run became forking until reaching the threshold of 65536 elements to call qsort(), with the lowest runtime of 0.108s.
This point represented diminishing returns, as runs with smaller thresholds had a longer real runtime of 0.118s for a 32768 threshold and 0.123s for a 16384 threshold.
As demonstrated by how sys time increased from 0.082s to 0.110s to 0.156s as the threshold decreased from 65536 to 32768 to 16384, this point of diminishing returns was likely due to the increased overhead with context switching and process control.
Additionally, the smaller thresholds would make each parallel task too small to offset the overhead costs of managing all of the parallel processes.

The increasing user time with each run also demonstrates how the program runs in parallel, as the user time is the sum of the CPU time used by each process.
The more processes running in parallel, the larger their sum of runtime would be, considering that starting each process also has some associated overhead.

The program was also run on a Lenovo laptop with fewer cores and computational power than the used ugrad machine to ensure continuity between the results for the same forking thresholds of elements in the array before a quicksort is called.

The results for the Lenovo laptop are shown below:

Test run with threshold 2097152
real    0m0.699s
user    0m0.603s
sys     0m0.030s

Test run with threshold 1048576
real    0m0.322s
user    0m0.632s
sys     0m0.065s

Test run with threshold 524288
real    0m0.227s
user    0m0.715s
sys     0m0.119s

Test run with threshold 262144
real    0m0.234s
user    0m0.813s
sys     0m0.119s

Test run with threshold 131072
real    0m0.167s
user    0m0.790s
sys     0m0.109s

Test run with threshold 65536
real    0m0.157s
user    0m0.730s
sys     0m0.144s

Test run with threshold 32768
real    0m0.196s
user    0m0.756s
sys     0m0.205s

Test run with threshold 16384
real    0m0.196s
user    0m0.868s
sys     0m0.715s

The results demonstrate that the laptop takes more time to run each of the programs, but the observed dynamics were the same.
The runtime decreased as the forking threshold decreased and the number of parallel processes increased, with the run threshold of 65536 having the best runtime.
This threshold was identical to that found above.

Implementing forking thresholds less than 65536 led to a similar diminishing of returns, indicated by an increasing real runtime. 
Additionally, the user time continually increased as the threshold decreased, indicating an increase in parallel processes.
It should be noted that for both machines, despite the Lenovo laptop starting with around 2 times more time for the runtime, the optimal runtime fell close to 1 second, demonstrating how optimizing the parallel processes can lead to an ideal limit.

Therefore, as indicated by the ugrad machine and Lenovo laptop experiments, this program effectively a parallel process control of the quicksort method, leading to a more effective performance.
For both machines, the best-performing threshold was 65536 elements in the child array before there would be no more forking, and the qsort() method would be called, indicating that an implementation of this parallel sorting should use that parameter for the best performance.