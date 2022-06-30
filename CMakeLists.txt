cmake_minimum_required(VERSION 3.10)

project(WonderMake VERSION 1.0.0 LANGUAGES CXX)

option(WonderMake_Testing OFF)
option(WonderMake_TestingManual OFF)
option(WonderMake_SetCxxStandard ON)
option(WonderMake_SetConfigurationTypes ON)
option(WonderMake_CopyDebugFolders ON)

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

set(WONDERMAKE_BIN_DIR "${CMAKE_BINARY_DIR}/bin/")

file(COPY "bin/." DESTINATION "${WONDERMAKE_BIN_DIR}")
if(WonderMake_CopyDebugFolders)
	set(WONDERMAKE_DEBUG_DATA_DIR "${CMAKE_BINARY_DIR}/data/")
	set(WONDERMAKE_DEBUG_USER_DIR "${CMAKE_BINARY_DIR}/user/")
	set(WONDERMAKE_DEBUG_USERDATA_DIR "${CMAKE_BINARY_DIR}/userdata/")

    file(COPY "bin-debug/." DESTINATION "${WONDERMAKE_BIN_DIR}")
    file(COPY "data-debug/." DESTINATION "${WONDERMAKE_DEBUG_DATA_DIR}")
    file(COPY "user-debug/." DESTINATION "${WONDERMAKE_DEBUG_USER_DIR}")
    file(COPY "userdata-debug/." DESTINATION "${WONDERMAKE_DEBUG_USERDATA_DIR}")
endif()
