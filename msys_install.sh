#!/bin/bash
#
# This script installs the applications required to build `host` directory on MSYS2 (64bit).
# It should be run only once after MSYS2 has been installed on Windows.
#
#  MSYS2: https://www.msys2.org/
#
#  1. Install MSYS2
#  2. Run MSYS2 application. (C:\msys64\mingw64.exe)
#  3. Goto this directory
#     $ cd <drag & drop directory>
#  4. Run this script
#     $ ./msys_install.sh
#  5. Build project
#     $ ./msys_build.sh
#
pacman --noconfirm -S \
  git \
  make \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-gcc
