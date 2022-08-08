cmake_minimum_required(VERSION 3.19)

include("${CMAKE_CURRENT_LIST_DIR}/conan.cmake")

conan_check(VERSION 1.45.0 REQUIRED)

conan_cmake_run(CONANFILE "conanfile.txt" UPDATE BUILD missing)

conan_basic_setup()

find_package(Protobuf REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/protobuf_helper.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/target_helper.cmake")
if(WonderMake_Testing)
    include("${CMAKE_CURRENT_LIST_DIR}/gtest.cmake")
endif()
