REM Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
REM All rights reserved.
REM This software is released under the MIT License.
REM http://opensource.org/licenses/mit-license.php

@echo off
@REM
@REM Build host test program for Windows
@REM

cd %~dp0
if not exist "build-host\" mkdir "build-host\"
cd build-host

if not exist "CMakeCache.txt" cmake ..\host

@REM TODO: Build script
