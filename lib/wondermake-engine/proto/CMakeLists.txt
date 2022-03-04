cmake_minimum_required(VERSION 3.19)

find_package(Protobuf REQUIRED)

list(APPEND ProtoFiles
    "ConfigurationRemote.proto"
    "LoggerRemote.proto")

add_protobuf_library(TARGET WonderMakeEngineProto FOLDER_NAME "wondermake-engine-proto" PROTOS ${ProtoFiles})

set_target_properties(WonderMakeEngineProto PROPERTIES FOLDER "proto")
