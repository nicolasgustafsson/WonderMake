cmake_minimum_required(VERSION 3.19)

find_package(Protobuf REQUIRED)

list(APPEND ProtoFiles
    "LoggerRemote.proto")

add_protobuf_library(WonderMakeEngineProto "wondermake-engine-proto" ${ProtoFiles})

set_target_properties(WonderMakeEngineProto PROPERTIES FOLDER "proto")
