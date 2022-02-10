cmake_minimum_required(VERSION 3.10)

include("conan.cmake")
include("TargetHelper.cmake")

if(WonderMake_Testing)
    include("dependencies/GTest.cmake")
endif()
