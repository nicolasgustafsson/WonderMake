cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Inc/catch2/catch.hpp")

add_library(Catch2 INTERFACE ${SourceFiles})

target_include_directories(Catch2 INTERFACE "Inc")
