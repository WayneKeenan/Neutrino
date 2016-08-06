#!/bin/bash
rm -rf CMake*
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../
