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
    "../WonderMakeBase/Source/Logger.cpp"
    "../WonderMakeBase/Source/WinEventSystemImpl.cpp"
    "../WonderMakeBase/Source/WinGuidGeneratorSystem.cpp"
    "../WonderMakeBase/Source/WinProcess.cpp"
    "../WonderMakeBase/Source/WinProcessSystem.cpp"
    "../WonderMakeUtility/Source/Time.cpp")
    
add_executable(WonderMakeBaseTests ${SourceFiles})

set_target_properties(WonderMakeBaseTests PROPERTIES FOLDER "wondermake/test")

add_test(WonderMakeBaseTests WonderMakeBaseTests)

target_link_libraries(WonderMakeBaseTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeBaseTests
    PUBLIC
        "Include"
        "../WonderMakeBase/Include"
        "../WonderMakeUtility/Include"
    PRIVATE
        "../WonderMakeBase/Source")

target_compile_definitions(WonderMakeBaseTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeBaseTests)
