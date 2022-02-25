cmake_minimum_required(VERSION 3.10)

add_subdirectory("proto")

list(APPEND SourceFiles
    "include/wondermake-engine/ConfigurationEngine.h"
    "include/wondermake-engine/DeserializeConfigurationJob.h"
    "include/wondermake-engine/LoggerFileSystem.h"
    "include/wondermake-engine/LoggerRemoteConnection.h"
    "include/wondermake-engine/LoggerRemoteSocket.h"
    "include/wondermake-engine/LoggerRemoteSystem.h"
    "include/wondermake-engine/SerializeConfigurationJob.h"
    "source/ConfigurationEngine.cpp"
    "source/DeserializeConfigurationJob.cpp"
    "source/LoggerFileSystem.cpp"
    "source/LoggerRemoteConnection.cpp"
    "source/LoggerRemoteMessage.cpp"
    "source/LoggerRemoteMessage.h"
    "source/LoggerRemoteSocket.cpp"
    "source/LoggerRemoteSystem.cpp"
    "source/SerializeConfigurationJob.cpp")

add_library(WonderMakeEngine STATIC ${SourceFiles})

target_include_directories(WonderMakeEngine PUBLIC "Include")
if(MSVC)
    target_link_options(WonderMakeEngine PUBLIC "/WHOLEARCHIVE:WonderMakeEngine")
endif()

target_link_libraries(WonderMakeEngine WonderMakeEngineProto WonderMakeBase WonderMakeUtility WonderMakeIo NlohmannJson)
