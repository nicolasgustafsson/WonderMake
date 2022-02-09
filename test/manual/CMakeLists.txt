cmake_minimum_required(VERSION 3.10)

add_subdirectory("wondermake-tests")
add_subdirectory("wondermake-io-tests-manual")

set_buildsystem_targets_folder("." "wondermake/test/manual")
