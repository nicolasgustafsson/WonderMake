cmake_minimum_required(VERSION 3.19)

include("${CMAKE_CURRENT_LIST_DIR}/conan.cmake")

conan_check(VERSION 1.51.3 REQUIRED)

if (WonderMake_OfflineMode)
    conan_cmake_run(CONANFILE "conanfile.txt"
                    BUILD missing
                    PROFILE_BUILD default
                    PROFILE default
                    PROFILE_AUTO build_type)
else()
    conan_cmake_run(CONANFILE "conanfile.txt"
                    UPDATE
                    BUILD missing
                    PROFILE_BUILD default
                    PROFILE default
                    PROFILE_AUTO build_type)
endif()

conan_basic_setup()

# Need to add the binary directory here, otherwise cmake can't find config packages.
LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(Protobuf REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

if(WonderMake_Testing)
    find_package(GTest REQUIRED)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/protobuf_helper.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/target_helper.cmake")
