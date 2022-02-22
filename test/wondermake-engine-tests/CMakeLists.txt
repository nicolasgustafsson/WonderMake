cmake_minimum_required(VERSION 3.10)
    
add_executable(WonderMakeEngineTests ${SourceFiles})

add_test(WonderMakeEngineTests WonderMakeEngineTests)

target_link_libraries(WonderMakeEngineTests gtest_main gmock WonderMakeTestsCommon NlohmannJson)
target_include_directories(WonderMakeEngineTests
    PUBLIC
        "include"

target_compile_definitions(WonderMakeEngineTests PUBLIC "WM_SUPPRESS_SYSTEM_GLOBAL" "WM_SUPPRESS_LOGGER" "WM_SUPPRESS_JOB_GLOBAL")

gtest_discover_tests(WonderMakeEngineTests)
