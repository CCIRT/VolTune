cmake_minimum_required(VERSION 3.0)

project(power)
set(CMAKE_CXX_STANDARD 17)

add_executable(power-measure
  src/main.cpp
  src/config.cpp
  src/args.cpp
)
target_link_libraries(power-measure PUBLIC util_lib xsdb_lib)
set_target_properties(power-measure
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
