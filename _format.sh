#!/bin/bash

#findscript=-type f '(' -name '*.cpp' -o -name '*.hpp' ')' -print -exec clang-format -i '{}' ';'

find . -maxdepth 1 -type f '(' -name '*.cpp' -o -name '*.hpp' ')' -print -exec clang-format -i '{}' ';'
find ./builtin_mods -type f '(' -name '*.cpp' -o -name '*.hpp' ')' -print -exec clang-format -i '{}' ';'
