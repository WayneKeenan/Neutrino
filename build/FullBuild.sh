#!/bin/bash
echo 'Cleaning project...'
echo ''
./CleanAll.sh
echo ''
echo 'Generating CMakes Files...'
echo ''
./GenCmake.sh
echo ''
echo 'Building....'
echo ''
./BuildAndRun.sh
