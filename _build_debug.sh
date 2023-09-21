#!/bin/bash

echo "------------------------------------------"
mkdir build_debug > /dev/null 2>&1
cd build_debug && cmake -DCMAKE_BUILD_TYPE=Debug .. && make "$@"
