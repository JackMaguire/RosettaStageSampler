#!/bin/bash

for x in {0..24}; do echo $x `grep -B1 Best $x.log | tail -n2 | head -n1`; done
