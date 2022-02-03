cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeTestsCommon/GTestInclude.h")

add_library(WonderMakeTestsCommon INTERFACE)

target_include_directories(WonderMakeTestsCommon INTERFACE "Include")
