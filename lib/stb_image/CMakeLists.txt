cmake_minimum_required(VERSION 3.0)
project(stb_image)

add_library(stb_image include/stb_image.h include/stb_image.cpp)
target_include_directories(stb_image PUBLIC include/)