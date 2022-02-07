cmake_minimum_required(VERSION 3.10)

# Inproper Unit-Tests has been disabled.
list(APPEND SourceFiles
#    "FileSystem/ReadFileJobTests.cpp"
    "Functionalities/TransformFunctionalityTests.cpp"
    "Job/JobMock.h"
    "Job/JobTests.cpp"
    "ObjectsAndPolicies/ObjectTests.cpp"
    "ObjectsAndPolicies/PolicyTests.cpp"
    "Utilities/MatrixTests.cpp"
    "Utilities/RotationTests.cpp"
    "Utilities/UniqueFunctionTests.cpp"
    "Utilities/VectorTests.cpp"
    "ContainerTests.cpp"
    "Main.cpp"
#    "ParseJsonJobTests.cpp"
    "SystemContainerTests.cpp"
    "SystemRegistryTests.cpp")
    
add_executable(WonderMakeTests ${SourceFiles})

set_target_properties(WonderMakeTests PROPERTIES FOLDER "wondermake/test/manual")

add_test(WonderMakeTests WonderMakeTests)

target_include_directories(WonderMakeTests
    PUBLIC
        "."
    PRIVATE
        "../WonderMakeIo/Source")
target_link_libraries(WonderMakeTests Catch2 WonderMake)

gtest_discover_tests(WonderMakeTests)
