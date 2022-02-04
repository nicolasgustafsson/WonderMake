cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeEngine/LoggerFileSystem.h"
    "Include/WonderMakeEngine/LoggerRemoteConnectionSystem.h"
    "Include/WonderMakeEngine/LoggerRemoteSocketSystem.h"
    "Source/LoggerFileSystem.cpp"
    "Source/LoggerRemoteConnectionSystem.cpp"
    "Source/LoggerRemoteMessageType.cpp"
    "Source/LoggerRemoteMessageType.h"
    "Source/LoggerRemoteSocketSystem.cpp")

add_library(WonderMakeEngine STATIC ${SourceFiles})

set_target_properties(WonderMakeEngine PROPERTIES FOLDER "wondermake/lib")

target_include_directories(WonderMakeEngine PUBLIC "Include")
if(MSVC)
    target_link_options(WonderMakeEngine PUBLIC "/WHOLEARCHIVE:WonderMakeEngine")
endif()

target_link_libraries(WonderMakeEngine WonderMakeBase WonderMakeUtility WonderMakeIo)
