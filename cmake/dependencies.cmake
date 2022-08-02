cmake_minimum_required(VERSION 3.19)

include("${CMAKE_CURRENT_LIST_DIR}/conan.cmake")

conan_check(VERSION 1.45.0 REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/dependencies/protobuf.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/TargetHelper.cmake")
if(WonderMake_Testing)
    include("${CMAKE_CURRENT_LIST_DIR}/dependencies/GTest.cmake")
endif()
