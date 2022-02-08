cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/catch2/catch.hpp")

add_library(Catch2 INTERFACE ${SourceFiles})

set_target_properties(Catch2 PROPERTIES FOLDER "wondermake/external")

target_include_directories(Catch2 INTERFACE "Inc")
