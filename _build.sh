#!/bin/bash

echo "------------------------------------------"
# python3 ./build_num.py
mkdir build > /dev/null 2>&1
cd build && cmake .. && make "$@"
