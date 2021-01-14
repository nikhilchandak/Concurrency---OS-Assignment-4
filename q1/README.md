# Concurrent Merge Sort

## Implementation

Merge sort is implemented via all three methods:

1. Normal Merge Sort
2. Merge Sort using processes
3. Merge Sort using threads

## Conclusion

It is observed that for small test case (N roughly 10-20) all three have similar performance and the 
difference is random and small although mostly normal mergesort seems to be the fastest.

On larger test cases like N = 1000, normal mergesort is 100x faster than both of the other variants 
with threaded_mergesort being the slowest (concurrent mergesort with processes runs (2-3)x faster than threaded).
