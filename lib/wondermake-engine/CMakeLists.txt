cmake_minimum_required(VERSION 3.19)

add_subdirectory("proto")

list(APPEND IncludeFiles
    "include/wondermake-engine/ConfigurationEngine.h"
    "include/wondermake-engine/ConfigurationRemoteSystem.h"
    "include/wondermake-engine/DeserializeConfigurationJob.h"
    "include/wondermake-engine/LoggerFileSystem.h"
    "include/wondermake-engine/LoggerRemoteConnection.h"
    "include/wondermake-engine/LoggerRemoteSocket.h"
    "include/wondermake-engine/LoggerRemoteSystem.h"
    "include/wondermake-engine/SerializeConfigurationJob.h")

list(APPEND SourceFiles
    "source/ConfigurationEngine.cpp"
    "source/ConfigurationProtoSerialization.cpp"
    "source/ConfigurationProtoSerialization.h"
    "source/ConfigurationRemoteSystem.cpp"
    "source/DeserializeConfigurationJob.cpp"
    "source/FileResourceLoader.cpp"
    "source/FileResourceLoader.h"
    "source/LoggerFileSystem.cpp"
    "source/LoggerRemoteConnection.cpp"
    "source/LoggerRemoteSocket.cpp"
    "source/LoggerRemoteSystem.cpp"
    "source/SerializeConfigurationJob.cpp")

list(APPEND TargetDependencies WonderMakeEngineProto WonderMakeBase WonderMakeUtility WonderMakeIo NlohmannJson)

if (NOT WonderMake_ImportLibraries)
    add_library(WonderMakeEngine STATIC ${IncludeFiles} ${SourceFiles})
    
    set_pedantic_warning_level(WonderMakeEngine)
    set_whole_archive(WonderMakeEngine PUBLIC)
    target_include_directories(WonderMakeEngine PUBLIC "include")
    target_link_libraries(WonderMakeEngine ${TargetDependencies})
else()
    add_library(WonderMakeEngine STATIC IMPORTED GLOBAL ${IncludeFiles})
    
    set_whole_archive(WonderMakeEngine INTERFACE)
    target_include_directories(WonderMakeEngine INTERFACE "include")
    target_link_libraries(WonderMakeEngine INTERFACE ${TargetDependencies})
endif()
