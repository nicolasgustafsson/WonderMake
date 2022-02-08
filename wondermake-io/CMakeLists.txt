cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeIo/FileSystem.h"
    "Include/WonderMakeIo/FileTypes.h"
    "Include/WonderMakeIo/IpcAcceptor.h"
    "Include/WonderMakeIo/IpcConnection.h"
    "Include/WonderMakeIo/IpcSystem.h"
    "Include/WonderMakeIo/Socket.h"
    "Include/WonderMakeIo/SocketDecorator.h"
    "Include/WonderMakeIo/SocketSerializing.h"
    "Include/WonderMakeIo/SocketSerializingImpl.h"
    "Source/SocketDecorator.cpp"
    "Source/WinFileSystem.cpp"
    "Source/WinFileSystem.h"
    "Source/WinIpcAcceptor.cpp"
    "Source/WinIpcAcceptor.h"
    "Source/WinIpcConnection.cpp"
    "Source/WinIpcConnection.h"
    "Source/WinIpcSystem.cpp"
    "Source/WinIpcSystem.h")

add_library(WonderMakeIo STATIC ${SourceFiles})

set_target_properties(WonderMakeIo PROPERTIES FOLDER "wondermake/lib")

target_include_directories(WonderMakeIo PUBLIC "Include")
if(MSVC)
    target_link_options(WonderMakeIo PUBLIC "/WHOLEARCHIVE:WonderMakeIo")
endif()

target_link_libraries(WonderMakeIo WonderMakeBase WonderMakeUtility)
