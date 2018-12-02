#!/bin/bash

case="main"
if [[ "$#" -gt "0" ]]; then
    case=$1
fi

#extra="-g -O0 -Wall"
#extra="-O3"

clang++ $case.cc -o $case -I. -std=c++1z $extra

