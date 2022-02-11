cmake_minimum_required(VERSION 3.10)

project(WonderMake VERSION 1.0.0 LANGUAGES CXX)

option(WonderMake_Testing OFF)
option(WonderMake_TestingManual OFF)
option(WonderMake_SetCxxStandard ON)
option(WonderMake_SetConfigurationTypes ON)

if(WonderMake_SetCxxStandard)
	set(CMAKE_CXX_STANDARD 20)
endif()

if(WonderMake_SetConfigurationTypes)
	SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
endif()

if(WonderMake_Testing)
	enable_testing()
endif()

add_subdirectory("cmake" EXCLUDE_FROM_ALL)

add_subdirectory("external")
add_subdirectory("lib")

if(WonderMake_Testing OR WonderMake_TestingManual)
	add_subdirectory("test")
endif()

set_buildsystem_targets_folder("." "wondermake")
