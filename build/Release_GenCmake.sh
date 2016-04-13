#!/bin/bash
rm -rf CMake*
cmake -D CMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../
