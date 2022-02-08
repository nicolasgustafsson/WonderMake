cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "include/wondermake-engine/LoggerFileSystem.h"
    "include/wondermake-engine/LoggerRemoteConnectionSystem.h"
    "include/wondermake-engine/LoggerRemoteSocketSystem.h"
    "source/LoggerFileSystem.cpp"
    "source/LoggerRemoteConnectionSystem.cpp"
    "source/LoggerRemoteMessageType.cpp"
    "source/LoggerRemoteMessageType.h"
    "source/LoggerRemoteSocketSystem.cpp")

add_library(WonderMakeEngine STATIC ${SourceFiles})

set_target_properties(WonderMakeEngine PROPERTIES FOLDER "wondermake/lib")

target_include_directories(WonderMakeEngine PUBLIC "Include")
if(MSVC)
    target_link_options(WonderMakeEngine PUBLIC "/WHOLEARCHIVE:WonderMakeEngine")
endif()

target_link_libraries(WonderMakeEngine WonderMakeBase WonderMakeUtility WonderMakeIo)
