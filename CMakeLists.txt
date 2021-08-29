# CMakeList.txt : CMake project for zpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("zpp")
set(CMAKE_CXX_STANDARD 17)
# Add source to this project's executable.
add_executable (zpp "zpp.cpp" "miniz.c")

# TODO: Add tests and install targets if needed.
