cmake_minimum_required(VERSION 3.19)

include("${CMAKE_CURRENT_LIST_DIR}/conan.cmake")

conan_check(VERSION 1.45.0 REQUIRED)

conan_cmake_run(CONANFILE "conanfile.txt"
                UPDATE
                BUILD missing
                PROFILE_BUILD default
                PROFILE default
                PROFILE_AUTO build_type)

conan_basic_setup()

# Without adding this, the magic_enum package can't be found by the cmake generator if not run by Visual Studio.
# The issue can be reproduced by commenting this line and running cmake. However it did not reproduce if run from a development console, which is probably why it works in Visual Studio.
# This should be investigated further, but works for now.
LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(Protobuf REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/protobuf_helper.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/target_helper.cmake")
if(WonderMake_Testing)
    include("${CMAKE_CURRENT_LIST_DIR}/gtest.cmake")
endif()
