cmake_minimum_required(VERSION 3.10)

add_subdirectory("imgui")
add_subdirectory("wondermake")
add_subdirectory("wondermake-base")
add_subdirectory("wondermake-engine")
add_subdirectory("wondermake-io")
add_subdirectory("wondermake-utility")

set_buildsystem_targets_folder("." "lib")
