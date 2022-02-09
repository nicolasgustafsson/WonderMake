cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeBaseTests/ProcessMock.h"
    "Include/WonderMakeBaseTests/WinEventSystemMock.h"
    "Include/WonderMakeBaseTests/WinPlatformSystemMock.h"
    "Tests/LoggerTests.cpp"
    "Tests/WinEventSystemImplTests.cpp"
    "Tests/WinGuidGeneratorSystemTests.cpp"
    "Tests/WinProcessSystemTests.cpp"
    "Tests/WinProcessTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-base/source/Logger.cpp"
    "../../lib/wondermake-base/source/WinEventSystemImpl.cpp"
    "../../lib/wondermake-base/source/WinGuidGeneratorSystem.cpp"
    "../../lib/wondermake-base/source/WinProcess.cpp"
    "../../lib/wondermake-base/source/WinProcessSystem.cpp"
    "../../lib/wondermake-utility/source/Time.cpp")
    
add_executable(WonderMakeBaseTests ${SourceFiles})

add_test(WonderMakeBaseTests WonderMakeBaseTests)

target_link_libraries(WonderMakeBaseTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeBaseTests
    PUBLIC
        "Include"
        "../../lib/wondermake-base/include"
        "../../lib/wondermake-utility/include"
    PRIVATE
        "../../lib/wondermake-base/source")

target_compile_definitions(WonderMakeBaseTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeBaseTests)
