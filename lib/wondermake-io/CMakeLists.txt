cmake_minimum_required(VERSION 3.19)

list(APPEND IncludeFiles
    "include/wondermake-io/ConfigurationIo.h"
    "include/wondermake-io/FileResource.h"
    "include/wondermake-io/FileResourceFactory.h"
    "include/wondermake-io/FileResourceProxy.h"
    "include/wondermake-io/FileResourceSystem.h"
    "include/wondermake-io/FileTypes.h"
    "include/wondermake-io/FileWatcherSystem.h"
    "include/wondermake-io/IpcAcceptor.h"
    "include/wondermake-io/IpcConnection.h"
    "include/wondermake-io/IpcSystem.h"
    "include/wondermake-io/PlatformFilePaths.h"
    "include/wondermake-io/ReadFileJob.h"
    "include/wondermake-io/Socket.h"
    "include/wondermake-io/SocketAcceptor.h"
    "include/wondermake-io/SocketDecorator.h"
    "include/wondermake-io/SocketProtobuf.h"
    "include/wondermake-io/SocketProtobufImpl.h"
    "include/wondermake-io/WriteFileJob.h")

list(APPEND SourceFiles
    "source/ConfigurationIo.cpp"
    "source/FileResource.cpp"
    "source/FileResourceSystem.cpp"
    "source/FileWatcherSystem.cpp"
    "source/ReadFileJob.cpp"
    "source/SocketDecorator.cpp"
    "source/WinFileWatcherSystem.cpp"
    "source/WinFileWatcherSystem.h"
    "source/WinIpcAcceptor.cpp"
    "source/WinIpcAcceptor.h"
    "source/WinIpcConnection.cpp"
    "source/WinIpcConnection.h"
    "source/WinIpcSystem.cpp"
    "source/WinIpcSystem.h"
    "source/WinPlatformFilePaths.cpp"
    "source/WriteFileJob.cpp")

list(APPEND TargetDependencies WonderMakeBase WonderMakeUtility magic_enum::magic_enum)

if (NOT WonderMake_ImportLibraries)
    add_library(WonderMakeIo STATIC ${IncludeFiles} ${SourceFiles})
    
    set_pedantic_warning_level(WonderMakeIo)
    set_whole_archive(WonderMakeIo PUBLIC)
    target_include_directories(WonderMakeIo PUBLIC "include")
    target_link_libraries(WonderMakeIo ${TargetDependencies})
else()
    add_library(WonderMakeIo STATIC IMPORTED GLOBAL ${IncludeFiles})
    
    set_whole_archive(WonderMakeIo INTERFACE)
    target_include_directories(WonderMakeIo INTERFACE "include")
    target_link_libraries(WonderMakeIo INTERFACE ${TargetDependencies})
endif()
