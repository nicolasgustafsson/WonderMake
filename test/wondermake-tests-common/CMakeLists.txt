cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "include/wondermake-tests-common/GTestInclude.h")

add_library(WonderMakeTestsCommon INTERFACE)

target_include_directories(WonderMakeTestsCommon INTERFACE "include")
