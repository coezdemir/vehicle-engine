cmake_minimum_required(VERSION 3.18)

project(Nuklear C)

# add glad as a static library
add_library(Nuklear INTERFACE)

# specify the include directory for this static libray
target_include_directories(Nuklear INTERFACE .)