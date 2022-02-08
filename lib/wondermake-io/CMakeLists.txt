cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "include/wondermake-io/FileSystem.h"
    "include/wondermake-io/FileTypes.h"
    "include/wondermake-io/IpcAcceptor.h"
    "include/wondermake-io/IpcConnection.h"
    "include/wondermake-io/IpcSystem.h"
    "include/wondermake-io/Socket.h"
    "include/wondermake-io/SocketDecorator.h"
    "include/wondermake-io/SocketSerializing.h"
    "include/wondermake-io/SocketSerializingImpl.h"
    "source/SocketDecorator.cpp"
    "source/WinFileSystem.cpp"
    "source/WinFileSystem.h"
    "source/WinIpcAcceptor.cpp"
    "source/WinIpcAcceptor.h"
    "source/WinIpcConnection.cpp"
    "source/WinIpcConnection.h"
    "source/WinIpcSystem.cpp"
    "source/WinIpcSystem.h")

add_library(WonderMakeIo STATIC ${SourceFiles})

set_target_properties(WonderMakeIo PROPERTIES FOLDER "wondermake/lib")

target_include_directories(WonderMakeIo PUBLIC "include")
if(MSVC)
    target_link_options(WonderMakeIo PUBLIC "/WHOLEARCHIVE:WonderMakeIo")
endif()

target_link_libraries(WonderMakeIo WonderMakeBase WonderMakeUtility)
