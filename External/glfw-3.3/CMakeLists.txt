cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Inc/GLFW/glfw3.h"
    "Inc/GLFW/glfw3native.h")

add_library(Glfw INTERFACE ${SourceFiles})

set_target_properties(Glfw PROPERTIES FOLDER "wondermake/external")

target_include_directories(Glfw INTERFACE "Inc")
