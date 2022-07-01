cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Inc/Glad/glad.h")

add_library(Glad INTERFACE ${SourceFiles})

target_include_directories(Glad INTERFACE "Inc")
