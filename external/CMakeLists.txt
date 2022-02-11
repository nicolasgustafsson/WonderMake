cmake_minimum_required(VERSION 3.10)

add_subdirectory("catch-2.12.1")
add_subdirectory("glad")
add_subdirectory("glfw-3.3")
add_subdirectory("json-3.5.0")
add_subdirectory("KHR")
add_subdirectory("SoLoud")
add_subdirectory("stb")

set_buildsystem_targets_folder("." "external")
