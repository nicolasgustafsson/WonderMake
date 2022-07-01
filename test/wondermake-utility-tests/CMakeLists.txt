cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Tests/AliasTests.cpp"
    "Tests/AnyExecutorTests.cpp"
    "Tests/BindableTests.cpp"
    "Tests/DependencyInjectionTests.cpp"
    "Tests/FutureTests.cpp"
    "Tests/GuidTests.cpp"
    "Tests/MemoryUnitTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-utility/source/AnyExecutor.cpp"
    "../../lib/wondermake-utility/source/DependencyInjector.cpp")
    
add_executable(WonderMakeUtilityTests ${SourceFiles})

add_test(WonderMakeUtilityTests WonderMakeUtilityTests)

target_link_libraries(WonderMakeUtilityTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeUtilityTests PUBLIC "../../lib/wondermake-utility/include")

gtest_discover_tests(WonderMakeUtilityTests)
