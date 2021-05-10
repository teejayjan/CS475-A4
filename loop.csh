#!/bin/csh

#array size
foreach n (1000 10000 50000 100000 500000 750000 1000000 2000000 4000000 6000000 8000000)
    echo ARRAY_SIZE = $n
    g++ -DARRAY_SIZE=$n -o simd simd.cpp -fopenmp
    ./simd
end