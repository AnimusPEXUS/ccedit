#!/bin/bash

find -type f -name '*.cpp' -name '*.hpp' -exec clang-format -i '{}' ';' -print
