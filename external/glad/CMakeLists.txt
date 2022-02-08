cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/Glad/glad.h")

add_library(Glad INTERFACE ${SourceFiles})

set_target_properties(Glad PROPERTIES FOLDER "wondermake/external")

target_include_directories(Glad INTERFACE "Inc")
