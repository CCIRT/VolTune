#!/bin/bash
mkdir -p build
cd build
cmake -G "MSYS Makefiles" ../host

echo "build voltage-measure"
make voltage-measure

echo "build power-measure"
make power-measure
