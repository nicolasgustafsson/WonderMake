cmake_minimum_required(VERSION 3.10)

find_package(Protobuf REQUIRED)

list(APPEND ProtoFiles
    "SocketProtobufTest.proto")

add_protobuf_library(TARGET WonderMakeIoTestsProto FOLDER_NAME "wondermake-io-tests/protobuf" PROTOS ${ProtoFiles})

set_target_properties(WonderMakeIoTestsProto PROPERTIES FOLDER "proto")
