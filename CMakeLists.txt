# CMakeList.txt : CMake project for nes, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

# Add source to this project's executable.
add_executable(main "src/main.cpp")

enable_testing()
add_executable(tester "tests/test.cpp")
add_test(Tester tester)