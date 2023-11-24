#!/bin/bash

cd build && strip ./codeedit && upx -9 ./codeedit && mv ./codeedit ./codeedit_stbl