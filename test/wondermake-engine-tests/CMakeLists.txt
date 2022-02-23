cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Tests/ConfigurationSerializationTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-base/source/Configuration.cpp"
    "../../lib/wondermake-base/source/ConfigurationSystem.cpp"
    "../../lib/wondermake-engine/source/DeserializeConfigurationJob.cpp"
    "../../lib/wondermake-engine/source/SerializeConfigurationJob.cpp")
    
add_executable(WonderMakeEngineTests ${SourceFiles})

add_test(WonderMakeEngineTests WonderMakeEngineTests)

target_link_libraries(WonderMakeEngineTests gtest_main gmock WonderMakeTestsCommon NlohmannJson)
target_include_directories(WonderMakeEngineTests
    PUBLIC
        "include"
        "../wondermake-base-tests/include"
        "../../lib/wondermake-base/include"
        "../../lib/wondermake-engine/include"
        "../../lib/wondermake-utility/include")

target_compile_definitions(WonderMakeEngineTests PUBLIC "WM_SUPPRESS_SYSTEM_GLOBAL" "WM_SUPPRESS_LOGGER" "WM_SUPPRESS_JOB_GLOBAL")

gtest_discover_tests(WonderMakeEngineTests)
