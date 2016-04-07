#!/bin/bash

if [ -e Makefile ]; then
    make clean
fi
rm -rf CMake*
rm -rf cmake*
rm -rf compile*
rm -rf Makefile
