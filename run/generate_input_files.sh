#!/bin/bash

grep -v num raw_data.dat > trajectories.dat

num_local_docks=94848
wc_to_expect=94849

while [[ `cat trajectories.dat | wc -l` -lt $wc_to_expect ]]; do
    #     m1     m2   m3   m4   m5   m6   m7  t1 t2 t3 t4 t5 t6 t7 t8
    echo "-9999 -9999 9999 9999 9999 9999 9999 0  0  0  0  0  0  0  0" >> trajectories.dat
done
