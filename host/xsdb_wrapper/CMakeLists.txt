cmake_minimum_required(VERSION 3.0)

project(xsdb_wrapper)
set(CMAKE_CXX_STANDARD 17)

# ------------------------------
# xsdb_lib
# ------------------------------
add_library(xsdb_lib
  STATIC
  src/xsdb.cpp
  src/pipe.cpp)
target_include_directories(xsdb_lib
  PUBLIC
    include/)


# ------------------------------
# sample
# ------------------------------
add_executable(sample_xsdb_program
  src/sample.cpp)

target_link_libraries(sample_xsdb_program xsdb_lib)
