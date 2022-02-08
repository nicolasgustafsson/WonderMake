cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Tests/WinIpcConnectionManualTests.cpp")
    
list(APPEND SourceFiles
    "../../../lib/wondermake-base/source/WinEventSystemImpl.cpp"
    "../../../lib/wondermake-base/source/WinPlatformSystemImpl.cpp"
    "../../../lib/wondermake-io/source/WinIpcAcceptor.cpp"
    "../../../lib/wondermake-io/source/WinIpcConnection.cpp")
    
add_executable(WonderMakeIoManualTests ${SourceFiles})

set_target_properties(WonderMakeIoManualTests PROPERTIES FOLDER "wondermake/test/manual")

add_test(WonderMakeIoManualTests WonderMakeIoManualTests)

target_link_libraries(WonderMakeIoManualTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeIoManualTests
    PUBLIC
        "include"
        "../wondermake-base-tests/include"
        "../../../lib/wondermake-base/include"
        "../../../lib/wondermake-io/include"
        "../../../lib/wondermake-utility/include"
    PRIVATE
        "../../../lib/wondermake-base/source"
        "../../../lib/wondermake-io/source")

target_compile_definitions(WonderMakeIoManualTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeIoManualTests)
