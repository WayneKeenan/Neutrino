#!/bin/bash
rm -rf CMake*
cmake -D CMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../
