cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/std/stb_image.h")

add_library(Stb INTERFACE ${SourceFiles})

target_include_directories(Stb INTERFACE "Inc")
