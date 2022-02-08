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
    "../wondermake-base/source/Logger.cpp"
    "../wondermake-base/source/WinEventSystemImpl.cpp"
    "../wondermake-base/source/WinGuidGeneratorSystem.cpp"
    "../wondermake-base/source/WinProcess.cpp"
    "../wondermake-base/source/WinProcessSystem.cpp"
    "../wondermake-utility/source/Time.cpp")
    
add_executable(WonderMakeBaseTests ${SourceFiles})

set_target_properties(WonderMakeBaseTests PROPERTIES FOLDER "wondermake/test")

add_test(WonderMakeBaseTests WonderMakeBaseTests)

target_link_libraries(WonderMakeBaseTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeBaseTests
    PUBLIC
        "Include"
        "../wondermake-base/include"
        "../wondermake-utility/include"
    PRIVATE
        "../wondermake-base/source")

target_compile_definitions(WonderMakeBaseTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeBaseTests)
