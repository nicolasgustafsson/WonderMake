cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Include/WonderMakeBaseTests/JobMock.h"
    "Include/WonderMakeBaseTests/ProcessMock.h"
    "Include/WonderMakeBaseTests/ProcessSystemMock.h"
    "Include/WonderMakeBaseTests/WinEventSystemMock.h"
    "Include/WonderMakeBaseTests/WinPlatformSystemMock.h"
    "Tests/ConfigurationSystemOverridesTests.cpp"
    "Tests/ConfigurationSystemTests.cpp"
    "Tests/ConfigurationTests.cpp"
    "Tests/ConfigurationTests.h"
    "Tests/JobRegistryTests.cpp"
    "Tests/JobSystemTests.cpp"
    "Tests/LoggerTests.cpp"
    "Tests/LogTagTests.cpp"
    "Tests/WinEventSystemImplTests.cpp"
    "Tests/WinGuidGeneratorSystemTests.cpp"
    "Tests/WinProcessSystemTests.cpp"
    "Tests/WinProcessTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-base/source/Configuration.cpp"
    "../../lib/wondermake-base/source/JobSystem.cpp"
    "../../lib/wondermake-base/source/WinEventSystemImpl.cpp"
    "../../lib/wondermake-base/source/WinGuidGeneratorSystem.cpp"
    "../../lib/wondermake-base/source/WinProcess.cpp"
    "../../lib/wondermake-base/source/WinProcessSystem.cpp"
    "../../lib/wondermake-utility/source/AnyExecutor.cpp"
    "../../lib/wondermake-utility/source/Time.cpp")
    
add_executable(WonderMakeBaseTests ${SourceFiles})

target_link_libraries(WonderMakeBaseTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeBaseTests
    PUBLIC
        "Include"
        "../wondermake-utility-tests/include"
        "../../lib/wondermake-base/include"
        "../../lib/wondermake-utility/include"
    PRIVATE
        "../../lib/wondermake-base/source")

target_compile_definitions(WonderMakeBaseTests PUBLIC "WM_SUPPRESS_SYSTEM_GLOBAL" "WM_SUPPRESS_LOGGER")

gtest_discover_tests(WonderMakeBaseTests)
