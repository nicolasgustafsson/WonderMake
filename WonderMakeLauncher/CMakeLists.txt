set(PROJECT_NAME WonderMakeLauncher)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Main.cpp"
    "pch.cpp"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE WonderMakeLauncher)

################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
        TARGET_NAME_RELEASE "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
        TARGET_NAME_RELEASE "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Bin/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Bin/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Bin/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Bin/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/home/nicos/Flibbon/Wondermake/Game;"
        "${CMAKE_CURRENT_SOURCE_DIR}/home/nicos/Flibbon/Wondermake/WonderMake;"
        "${CMAKE_CURRENT_SOURCE_DIR}/."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/home/nicos/Flibbon/Wondermake/Game;"
        "${CMAKE_CURRENT_SOURCE_DIR}/home/nicos/Flibbon/Wondermake/WonderMake;"
        "${CMAKE_CURRENT_SOURCE_DIR}/."
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /JMC;
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /MP;
            /std:c++latest;
            /sdl;
            /W4;
            /WX;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /w44062;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /JMC;
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /MP;
            /std:c++latest;
            /sdl;
            /W4;
            /WX;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /w44062;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Release>:
                /DEBUG:FASTLINK;
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE;
            /WHOLEARCHIVE:WonderMake_$(Platform)_$(Configuration);
            /WHOLEARCHIVE:Game_$(Platform)_$(Configuration);
            /ignore:4099
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Release>:
                /DEBUG:FASTLINK;
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE;
            /WHOLEARCHIVE:WonderMake_$(Platform)_$(Configuration);
            /WHOLEARCHIVE:Game_$(Platform)_$(Configuration);
            /ignore:4099
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    Game
    WonderMake
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3_x64_Release;"
        "soloud_x64_Release"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3_x86_Release;"
        "soloud_x86_Release"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

