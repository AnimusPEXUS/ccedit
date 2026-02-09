#!/bin/bash

echo "------------------------------------------"
# python3 ./build_num.py
mkdir build > /dev/null 2>&1
#export CXX=`which clang++`
#export CC=`which clang`
cd build && cmake .. && make "$@"
