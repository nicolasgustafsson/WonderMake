cmake_minimum_required(VERSION 3.19)

add_subdirectory("wondermake-tests-common")

if(WonderMake_Testing)
	add_subdirectory("wondermake-utility-tests")
	add_subdirectory("wondermake-base-tests")
	add_subdirectory("wondermake-io-tests")
	add_subdirectory("wondermake-engine-tests")
endif()

if(WonderMake_TestingManual)
	add_subdirectory("manual")
endif()

set_buildsystem_targets_folder("." "test")
