cmake_minimum_required(VERSION 3.19)

include("conan.cmake")

include("protobuf.cmake")
include("TargetHelper.cmake")

if(WonderMake_Testing)
    include("dependencies/GTest.cmake")
endif()
