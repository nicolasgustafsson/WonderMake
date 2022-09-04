cmake_minimum_required(VERSION 3.19)

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

set_pedantic_warning_level(WonderMakeEngine)
set_whole_archive(WonderMakeEngine)
target_include_directories(WonderMakeEngine PUBLIC "Include")

target_link_libraries(WonderMakeEngine WonderMakeEngineProto WonderMakeBase WonderMakeUtility WonderMakeIo NlohmannJson)
