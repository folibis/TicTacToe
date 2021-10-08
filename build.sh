#!/bin/bash

mkdir .build
cd .build
rm ../CMakeCache.txt
cmake .. -DCMAKE_BUILD_TYPE=Release
make
cd ..