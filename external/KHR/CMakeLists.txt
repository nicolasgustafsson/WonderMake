cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/KHR/khrplatform.h")

add_library(Khr INTERFACE ${SourceFiles})

set_target_properties(Khr PROPERTIES FOLDER "wondermake/external")

target_include_directories(Khr INTERFACE "Inc")
