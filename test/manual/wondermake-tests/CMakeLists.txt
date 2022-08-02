cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Functionalities/TransformFunctionalityTests.cpp"
    "ObjectsAndPolicies/ObjectTests.cpp"
    "ObjectsAndPolicies/PolicyTests.cpp"
    "Utilities/MatrixTests.cpp"
    "Utilities/RotationTests.cpp"
    "Utilities/UniqueFunctionTests.cpp"
    "Utilities/VectorTests.cpp"
    "ContainerTests.cpp"
    "Main.cpp"
    "SystemContainerTests.cpp"
    "SystemRegistryTests.cpp")
    
add_executable(WonderMakeTests ${SourceFiles})

add_test(WonderMakeTests WonderMakeTests)

target_include_directories(WonderMakeTests
    PUBLIC
        "."
    PRIVATE
        "../WonderMakeIo/Source")
target_link_libraries(WonderMakeTests Catch2 WonderMake)
