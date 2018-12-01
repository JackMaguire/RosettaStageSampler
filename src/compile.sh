#!/bin/bash

case="main"
if [[ "$#" -gt "0" ]]; then
    case=$1
fi

clang++ $case.cc -o $case -I. -std=c++1z -O3
