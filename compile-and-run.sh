#!/bin/bash

set -e

echo -n 'Compiling... '
clang++ \
    -std=c++17 -stdlib=libc++ -O3 \
    -F/Library/Frameworks -framework SDL2 \
        src/main.cpp src/vec2.hpp \
        src/vec3.hpp src/mat3.hpp \
        src/distances.hpp src/transformations.hpp

    
echo -n 'Running... '
./a.out
echo 'Done.'