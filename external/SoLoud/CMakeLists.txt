cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/soloud_audiosource.h")

add_library(SoLoud INTERFACE ${SourceFiles})

target_include_directories(SoLoud INTERFACE "Inc")
