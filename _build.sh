#!/bin/bash

echo "------------------------------------------"
mkdir build > /dev/null 2>&1
cd build && cmake .. && make -j4
