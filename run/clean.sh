#!/bin/bash

ls -1t * | grep -v or | grep -v raw | grep -v .sh | xargs -n1 rm
