#!/bin/bash
# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

mkdir -p build
cd build
cmake -G "MSYS Makefiles" ../host

echo "build voltage-measure"
make voltage-measure

echo "build power-measure"
make power-measure
