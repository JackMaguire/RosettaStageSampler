#!/bin/bash

case="main"
if [[ "$#" -gt "0" ]]; then
    case=$1
fi

extra="-g -O0 -Wall"
#extra="-Ofast"
#extra="-O3"

CXX="g++-5"
#CXX="clang++"

$CXX $case.cc -o $case -I. -std=c++1z $extra

