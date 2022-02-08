cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Tests/WinIpcConnectionManualTests.cpp")
    
list(APPEND SourceFiles
    "../wondermake-base/source/WinEventSystemImpl.cpp"
    "../wondermake-base/source/WinPlatformSystemImpl.cpp"
    "../wondermake-io/source/WinIpcAcceptor.cpp"
    "../wondermake-io/source/WinIpcConnection.cpp")
    
add_executable(WonderMakeIoManualTests ${SourceFiles})

set_target_properties(WonderMakeIoManualTests PROPERTIES FOLDER "wondermake/test/manual")

add_test(WonderMakeIoManualTests WonderMakeIoManualTests)

target_link_libraries(WonderMakeIoManualTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeIoManualTests
    PUBLIC
        "include"
        "../wondermake-base/include"
        "../wondermake-base-tests/include"
        "../wondermake-io/include"
        "../wondermake-utility/include"
    PRIVATE
        "../wondermake-base/source"
        "../wondermake-io/source")

target_compile_definitions(WonderMakeIoManualTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeIoManualTests)
