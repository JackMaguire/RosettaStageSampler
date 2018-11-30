#!/bin/bash

extra="-g -O0 -Wall"
#extra="-O3"

clang++ main.cc -o main -I. -std=c++1z $extra
#    -O3
