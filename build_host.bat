@echo off
@REM
@REM Build host test program for Windows
@REM

cd %~dp0
if not exist "build-host\" mkdir "build-host\"
cd build-host

if not exist "CMakeCache.txt" cmake ..\host

@REM TODO: Build script
