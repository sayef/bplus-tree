cmake_minimum_required(VERSION 3.15)
project(bplustree)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_COMPILER /opt/homebrew/bin/g++-12)
set(CMAKE_CXX_COMPILER /opt/homebrew/bin/g++-12)
include_directories(/usr/local/include)
add_executable(BPlusTree BPlusTree.cpp)

# cmake . && make && ./BPlusTree
