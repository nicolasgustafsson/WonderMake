cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Tests/WinIpcConnectionManualTests.cpp")
    
list(APPEND SourceFiles
    "../WonderMakeBase/Source/WinEventSystemImpl.cpp"
    "../WonderMakeBase/Source/WinPlatformSystemImpl.cpp"
    "../WonderMakeIo/Source/WinIpcAcceptor.cpp"
    "../WonderMakeIo/Source/WinIpcConnection.cpp")
    
add_executable(WonderMakeIoManualTests ${SourceFiles})

set_target_properties(WonderMakeIoManualTests PROPERTIES FOLDER "wondermake/test/manual")

add_test(WonderMakeIoManualTests WonderMakeIoManualTests)

target_link_libraries(WonderMakeIoManualTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeIoManualTests
    PUBLIC
        "Include"
        "../WonderMakeBase/Include"
        "../WonderMakeBaseTests/Include"
        "../WonderMakeIo/Include"
        "../WonderMakeUtility/Include"
    PRIVATE
        "../WonderMakeBase/Source"
        "../WonderMakeIo/Source")

target_compile_definitions(WonderMakeIoManualTests PUBLIC "WM_SUPRESS_SYSTEM_GLOBAL")

gtest_discover_tests(WonderMakeIoManualTests)
