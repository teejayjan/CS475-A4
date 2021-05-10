#!/bin/csh

#number of threads
foreach t (1 2 4 8 16)
    echo NUMT = $t
    #array size
    foreach n (1000 10000 50000 100000 500000 750000 1000000 2000000 4000000 6000000 8000000)
        echo ARRAY_SIZE = $n
        g++ -DARRAY_SIZE=$n -DNUM$t -o multisimd multisimd.cpp -fopenmp
        ./multisimd
    end
end