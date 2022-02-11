cmake_minimum_required(VERSION 3.10)

add_subdirectory("proto")

list(APPEND SourceFiles
    "include/wondermake-engine/LoggerFileSystem.h"
    "include/wondermake-engine/LoggerRemoteConnectionSystem.h"
    "include/wondermake-engine/LoggerRemoteSocketSystem.h"
    "source/LoggerFileSystem.cpp"
    "source/LoggerRemoteConnectionSystem.cpp"
    "source/LoggerRemoteMessage.cpp"
    "source/LoggerRemoteMessage.h"
    "source/LoggerRemoteSocketSystem.cpp")

add_library(WonderMakeEngine STATIC ${SourceFiles})

target_include_directories(WonderMakeEngine PUBLIC "Include")
if(MSVC)
    target_link_options(WonderMakeEngine PUBLIC "/WHOLEARCHIVE:WonderMakeEngine")
endif()

target_link_libraries(WonderMakeEngine WonderMakeEngineProto WonderMakeBase WonderMakeUtility WonderMakeIo)
