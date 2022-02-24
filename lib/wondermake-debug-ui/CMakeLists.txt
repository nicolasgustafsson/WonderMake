cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "include/wondermake-debug-ui/DebugSettingsSystem.h"
    "include/wondermake-debug-ui/ImguiInclude.h"
    "source/DebugSettingsSystem.cpp"
    "source/JsonInspector.cpp"
    "source/JsonInspector.h")

add_library(WonderMakeDebugUi STATIC ${SourceFiles})

set_pedantic_warning_level(WonderMakeDebugUi)
set_whole_archive(WonderMakeDebugUi)
target_include_directories(WonderMakeDebugUi PUBLIC "include")

target_link_libraries(WonderMakeDebugUi Imgui WonderMakeBase WonderMakeEngine WonderMakeIo WonderMakeUtility)
