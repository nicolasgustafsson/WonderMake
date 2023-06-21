cmake_minimum_required(VERSION 3.10)

list(APPEND IncludeFiles
    "include/wondermake-debug-ui/DebugSettingsSystem.h"
    "include/wondermake-debug-ui/DebugSystem.h"
    "include/wondermake-debug-ui/ImguiInclude.h"
    "include/wondermake-debug-ui/ImguiWrapper.h")

list(APPEND SourceFiles
    "source/ConfigurationDebugUi.cpp"
    "source/ConfigurationDebugUi.h"
    "source/ConfigurationView.cpp"
    "source/ConfigurationView.h"
    "source/DebugSettingsSystem.cpp"
    "source/ImGuiLogger.cpp"
    "source/ImGuiLogger.h"
    "source/DebugSystem.cpp"
    "source/ImguiWrapper.cpp"
    "source/JsonInspector.cpp"
    "source/JsonInspector.h")

list(APPEND TargetDependencies Imgui WonderMakeBase WonderMakeEngine WonderMakeIo WonderMakeUtility WonderMakeUi)

if (NOT WonderMake_ImportLibraries)
    add_library(WonderMakeDebugUi STATIC ${IncludeFiles} ${SourceFiles})
    
    set_pedantic_warning_level(WonderMakeDebugUi)
    set_whole_archive(WonderMakeDebugUi PUBLIC)
    target_include_directories(WonderMakeDebugUi PUBLIC "include")
    target_link_libraries(WonderMakeDebugUi ${TargetDependencies})
else()
    add_library(WonderMakeDebugUi STATIC IMPORTED GLOBAL ${IncludeFiles})
    
    set_whole_archive(WonderMakeDebugUi INTERFACE)
    target_include_directories(WonderMakeDebugUi INTERFACE "include")
    target_link_libraries(WonderMakeDebugUi INTERFACE ${TargetDependencies})
endif()
