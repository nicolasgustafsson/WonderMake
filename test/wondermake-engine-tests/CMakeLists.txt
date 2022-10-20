cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Tests/ConfigurationProtoSerializationTest.cpp"
    "Tests/ConfigurationRemoteSystemDressRehearsalTests.cpp"
    "Tests/ConfigurationRemoteSystemTests.cpp"
    "Tests/ConfigurationRemoteSystemTests.h"
    "Tests/ConfigurationSerializationTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-base/source/Configuration.cpp"
    "../../lib/wondermake-base/source/Logger.cpp"
    "../../lib/wondermake-engine/source/ConfigurationProtoSerialization.cpp"
    "../../lib/wondermake-engine/source/ConfigurationRemoteSystem.cpp"
    "../../lib/wondermake-engine/source/DeserializeConfigurationJob.cpp"
    "../../lib/wondermake-engine/source/SerializeConfigurationJob.cpp"
    "../../lib/wondermake-utility/source/AnyExecutor.cpp"
    "../../lib/wondermake-utility/source/Time.cpp")
    
add_executable(WonderMakeEngineTests ${SourceFiles})

target_link_libraries(WonderMakeEngineTests gtest_main gmock WonderMakeEngineProto WonderMakeTestsCommon NlohmannJson)
target_include_directories(WonderMakeEngineTests
    PUBLIC
        "include"
        "../wondermake-base-tests/include"
        "../wondermake-io-tests/include"
        "../../lib/wondermake-base/include"
        "../../lib/wondermake-engine/include"
        "../../lib/wondermake-io/include"
        "../../lib/wondermake-utility/include"
    PRIVATE
        "../../lib/wondermake-engine/source")

target_compile_definitions(WonderMakeEngineTests PUBLIC "WM_SUPPRESS_SYSTEM_GLOBAL" "WM_SUPPRESS_LOGGER" "WM_SUPPRESS_JOB_GLOBAL")

gtest_discover_tests(WonderMakeEngineTests)
