cmake_minimum_required(VERSION 3.19)

add_subdirectory("proto")

list(APPEND SourceFiles
    "Include/WonderMakeIoTests/ReadFileJobMock.h"
    "Include/WonderMakeIoTests/SocketAcceptorMock.h"
    "Include/WonderMakeIoTests/SocketMock.h"
    "Include/WonderMakeIoTests/WriteFileJobMock.h"
    "Tests/SocketDecoratorTests.cpp"
    "Tests/SocketProtobufImplTests.cpp"
    "Tests/WinIpcAcceptorTests.cpp"
    "Tests/WinIpcConnectionTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-base/source/Configuration.cpp"
    "../../lib/wondermake-base/source/Logger.cpp"
    "../../lib/wondermake-io/source/SocketDecorator.cpp"
    "../../lib/wondermake-io/source/WinIpcAcceptor.cpp"
    "../../lib/wondermake-io/source/WinIpcConnection.cpp"
    "../../lib/wondermake-utility/source/AnyExecutor.cpp"
    "../../lib/wondermake-utility/source/Time.cpp")
    
add_executable(WonderMakeIoTests ${SourceFiles})

target_link_libraries(WonderMakeIoTests gtest_main gmock WonderMakeTestsCommon WonderMakeIoTestsProto)
target_include_directories(WonderMakeIoTests
    PUBLIC
        "include"
        "../wondermake-base-tests/include"
        "../../lib/wondermake-base/include"
        "../../lib/wondermake-io/include"
        "../../lib/wondermake-utility/include"
    PRIVATE
        "../../lib/wondermake-io/source")

target_compile_definitions(WonderMakeIoTests PUBLIC "WM_SUPPRESS_SYSTEM_GLOBAL" "WM_SUPPRESS_LOGGER")

gtest_discover_tests(WonderMakeIoTests)
