# KD-Tree
KD-Tree implementation in C++ for 2D points

## Compilation:
`make debug` for debug build
`make release` for release build
`make clean` to clean up compiled units

## Execution:
`./kdtree < "directives-file-name"` or you can skip redirection and enter directives on stdin

## Known bugs or limitations:
- No two points should have same X or same Y coordinates
- Program fails if a branch is removed but the other one have depth greater than 2

## Notes:
Key point of constructing the KDTree in O(n log n) time is finding the median and partitioning the tree in O(n) time.
I've used std::nth_element() function of C++ Standard Library for this task. After executing this method, two things happen:
1. nth smallest element of the unsorted array is now at the nth index.
2. All elements smaller than the nth element are now on the left of it, and all larger elements are on the right of it.
This is done in O(n) average time using Quickselect (kth smallest) algorithm. Worst case can be O(n^2) just like Quicksort.
