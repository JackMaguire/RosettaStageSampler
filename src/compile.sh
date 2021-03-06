#!/bin/bash

case="main"
if [[ "$#" -gt "0" ]]; then
    case=$1
fi

#extra="-g -ggdb -Wall -gdwarf-3"
extra="-Ofast"
#extra="-O3"

CXX="g++-5"
#CXX="clang++"

omp="-fopenmp -lpthread"

echo $CXX $case.cc -o $case -I. -std=c++1z $extra $omp
$CXX $case.cc -o $case -I. -std=c++1z $extra $omp

