cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "include/wondermake-io/ConfigurationIo.h"
    "include/wondermake-io/FileSystem.h"
    "include/wondermake-io/FileTypes.h"
    "include/wondermake-io/IpcAcceptor.h"
    "include/wondermake-io/IpcConnection.h"
    "include/wondermake-io/IpcSystem.h"
    "include/wondermake-io/ReadFileJob.h"
    "include/wondermake-io/Socket.h"
    "include/wondermake-io/SocketDecorator.h"
    "include/wondermake-io/SocketSerializing.h"
    "include/wondermake-io/SocketSerializingImpl.h"
    "include/wondermake-io/WriteFileJob.h"
    "source/ConfigurationIo.cpp"
    "source/ReadFileJob.cpp"
    "source/SocketDecorator.cpp"
    "source/WinFileSystem.cpp"
    "source/WinFileSystem.h"
    "source/WinIpcAcceptor.cpp"
    "source/WinIpcAcceptor.h"
    "source/WinIpcConnection.cpp"
    "source/WinIpcConnection.h"
    "source/WinIpcSystem.cpp"
    "source/WinIpcSystem.h"
    "source/WriteFileJob.cpp")

add_library(WonderMakeIo STATIC ${SourceFiles})

set_pedantic_warning_level(WonderMakeIo)
set_whole_archive(WonderMakeIo)
target_include_directories(WonderMakeIo PUBLIC "include")

target_link_libraries(WonderMakeIo WonderMakeBase WonderMakeUtility magic_enum::magic_enum)
