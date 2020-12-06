#!/bin/bash

set -e

echo -n 'Compiling... '
clang++ src/main.cpp src/vec2.hpp src/vec3.hpp src/mat3.hpp -std=c++14 -O3
echo -n 'Running... '
./a.out
echo 'Done.'
open image.ppm