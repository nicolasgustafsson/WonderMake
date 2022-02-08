cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Tests/AliasTests.cpp"
    "Tests/BindableTests.cpp"
    "Tests/DependencyInjectionTests.cpp"
    "Tests/GuidTests.cpp"
    "Tests/MemoryUnitTests.cpp")
    
list(APPEND SourceFiles
    "../wondermake-utility/source/DependencyInjector.cpp")
    
add_executable(WonderMakeUtilityTests ${SourceFiles})

set_target_properties(WonderMakeUtilityTests PROPERTIES FOLDER "wondermake/test")

add_test(WonderMakeUtilityTests WonderMakeUtilityTests)

target_link_libraries(WonderMakeUtilityTests gtest_main gmock WonderMakeTestsCommon)
target_include_directories(WonderMakeUtilityTests PUBLIC "../wondermake-utility/include")

gtest_discover_tests(WonderMakeUtilityTests)
