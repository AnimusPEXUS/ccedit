#!/bin/bash

echo "------------------------------------------"
mkdir build > /dev/null 2>&1
cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make
