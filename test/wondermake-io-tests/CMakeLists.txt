cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeIoTests/SocketMock.h"
    "Include/WonderMakeIoTests/SocketSerializingMock.h"
    "Tests/SocketDecoratorTests.cpp"
    "Tests/SocketSerializingImplTests.cpp"
    "Tests/WinFileSystemTests.cpp"
    "Tests/WinIpcConnectionTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-io/source/SocketDecorator.cpp"
    "../../lib/wondermake-io/source/WinFileSystem.cpp"
    "../../lib/wondermake-io/source/WinIpcConnection.cpp")
    
add_executable(WonderMakeIoTests ${SourceFiles})

add_test(WonderMakeIoTests WonderMakeIoTests)

target_link_libraries(WonderMakeIoTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeIoTests
    PUBLIC
        "include"
        "../wondermake-base-tests/include"
        "../../lib/wondermake-base/include"
        "../../lib/wondermake-io/include"
        "../../lib/wondermake-utility/include"
    PRIVATE
        "../../lib/wondermake-io/source")

target_compile_definitions(WonderMakeIoTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeIoTests)
