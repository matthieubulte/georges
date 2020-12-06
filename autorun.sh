#!/bin/bash

fswatch -o ./src/*.cpp ./src/*.hpp | xargs -n1 -I{} ./compile-and-run.sh