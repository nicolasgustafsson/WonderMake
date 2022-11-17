cmake_minimum_required(VERSION 3.19)

project(WonderMake VERSION 1.0.0 LANGUAGES CXX)

option(WonderMake_Main					"Toggles all WonderMake engine libraries."					ON)
option(WonderMake_Testing				"Toggles all Unit Tests."									OFF)
option(WonderMake_TestingManual			"Toggles all Manual Tests."									OFF)
option(WonderMake_TestingLegacy			"Toggles all Legacy Unit Tests."							OFF)
option(WonderMake_SetCxxStandard		"Makes CMake set the c++ standard."							ON)
option(WonderMake_SetConfigurationTypes	"Makes CMake set the configurations."						ON)

# Turning this on makes it so most WonderMake targets use INTERFACE instead of STATIC when calling
# add_library, meaning that it won't link against any libraries. You'll need to supply the static
# libraries yourself. This is done by setting the IMPORTED_LOCATION property of these targets:
# Imgui WonderMake WonderMakeBase WonderMakeDebugUi WonderMakeEngine WonderMakeIo WonderMakeUtility
#
# Note that this not apply to Protobuf targets. They are still compiled and linked towards.
#
# The reason for this is because I was unable to get Github Actions to perform incremental
# compiling when caching the output. So this is far more intrusive than I would have wanted.
#
option(WonderMake_ImportLibraries		"Makes WonderMake import target libraries."					OFF)

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
endif()
