cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "include/wondermake-utility-tests/ExecutorMock.h"
    "Tests/AliasTests.cpp"
    "Tests/AnyExecutorTests.cpp"
    "Tests/BindableTests.cpp"
    "Tests/BufferExecutorTests.cpp"
    "Tests/CancelableListTests.cpp"
    "Tests/DependencyInjectionTests.cpp"
    "Tests/EventSubscriberTests.cpp"
    "Tests/FilePathTests.cpp"
    "Tests/FixedMapTests.cpp"
    "Tests/FixedSizeStringTests.cpp"
    "Tests/FlagSetTests.cpp"
    "Tests/FutureTests.cpp"
    "Tests/GuidTests.cpp"
    "Tests/MemoryUnitTests.cpp"
    "Tests/ResultTests.cpp"
    "Tests/StopwatchTests.cpp"
    "Tests/TimerManagerTests.cpp"
    "Tests/UniqueFunctionTests.cpp")
    
list(APPEND SourceFiles
    "../../lib/wondermake-utility/source/AnyExecutor.cpp"
    "../../lib/wondermake-utility/source/DependencyInjector.cpp")
    
add_executable(WonderMakeUtilityTests ${SourceFiles})

target_link_libraries(WonderMakeUtilityTests GTest::gtest_main GTest::gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeUtilityTests
    PUBLIC
        "include"
        "../../lib/wondermake-utility/include")

gtest_discover_tests(WonderMakeUtilityTests)
