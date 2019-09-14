# skiplist

A free Skiplist library written in C++.

This library provides an enhanced Skiplist which has the following features:

1. Support duplicate key insertion.
2. Support range search.

The probability of the random level generator implemented in this Skiplist is 1/4, i.e. every node has level 0, 1 in 4 has level 1, 1 in 16 has level 2, 1 in 64 has level 3, etc, achiving a complexity of O(log(n)).

