cmake_minimum_required(VERSION 3.19)

project(WonderMake VERSION 1.0.0 LANGUAGES CXX)

option(WonderMake_Main					"Toggles all WonderMake engine libraries."					ON)
option(WonderMake_Testing				"Toggles all Unit Tests."									OFF)
option(WonderMake_TestingManual			"Toggles all Manual Tests."									OFF)
option(WonderMake_TestingLegacy			"Toggles all Legacy Unit Tests."							OFF)
option(WonderMake_SetCxxStandard		"Makes CMake set the c++ standard."							ON)
option(WonderMake_SetConfigurationTypes	"Makes CMake set the configurations."						ON)
option(WonderMake_CopyDebugFolders		"Makes CMake copy the debug folders into the bin folder"	ON)

if(WonderMake_SetCxxStandard)
	set(CMAKE_CXX_STANDARD 20)
endif()

if(WonderMake_SetConfigurationTypes)
	SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
endif()

set(WONDERMAKE_BIN_DIR "${CMAKE_BINARY_DIR}/bin/")

if(WonderMake_Testing)
	enable_testing()
endif()

include("cmake/dependencies.cmake")

add_subdirectory("external")
# The engine libraries are required by the legacy tests.
if(WonderMake_Main OR WonderMake_TestingLegacy)
	add_subdirectory("lib")
endif()

if(WonderMake_Testing OR WonderMake_TestingManual OR WonderMake_TestingLegacy)
	add_subdirectory("test")
endif()

set_buildsystem_targets_folder("." "wondermake")

if(WonderMake_Main OR WonderMake_TestingLegacy)
	file(COPY "bin/." DESTINATION "${WONDERMAKE_BIN_DIR}")
	if(WonderMake_CopyDebugFolders)
		set(WONDERMAKE_DEBUG_DATA_DIR "${CMAKE_BINARY_DIR}/data/")
		set(WONDERMAKE_DEBUG_USER_DIR "${CMAKE_BINARY_DIR}/user/")
		set(WONDERMAKE_DEBUG_USERDATA_DIR "${CMAKE_BINARY_DIR}/userdata/")

		if(EXISTS "bin-debug/.")
			file(COPY "bin-debug/." DESTINATION "${WONDERMAKE_BIN_DIR}")
		endif()
		if(EXISTS "data-debug/.")
			file(COPY "data-debug/." DESTINATION "${WONDERMAKE_DEBUG_DATA_DIR}")
		endif()
		if(EXISTS "user-debug/.")
			file(COPY "user-debug/." DESTINATION "${WONDERMAKE_DEBUG_USER_DIR}")
		endif()
		if(EXISTS "userdata-debug/.")
			file(COPY "userdata-debug/." DESTINATION "${WONDERMAKE_DEBUG_USERDATA_DIR}")
		endif()
	endif()
endif()
