cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Inc/KHR/khrplatform.h")

add_library(Khr INTERFACE ${SourceFiles})

target_include_directories(Khr INTERFACE "Inc")
