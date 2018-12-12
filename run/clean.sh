#!/bin/bash

ls -1p * | grep -v / | grep -v or | grep -v raw | grep -v .sh | while read x; do rm $x; done
