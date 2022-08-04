cmake_minimum_required(VERSION 3.19)

add_subdirectory("glad")
add_subdirectory("glfw-3.3")
add_subdirectory("json-3.5.0")
add_subdirectory("KHR")
add_subdirectory("SoLoud")
add_subdirectory("stb")

# Catch2 is only used by the legacy unit tests
if(WonderMake_TestingLegacy)
    add_subdirectory("catch-2.12.1")
endif()

set_buildsystem_targets_folder("." "external")
