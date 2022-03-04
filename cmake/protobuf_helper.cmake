
conan_cmake_run(REQUIRES "protobuf/3.19.2" BASIC_SETUP)

find_package(Protobuf REQUIRED)

function(add_protobuf_library)
    set(options)
    set(args TARGET FOLDER_NAME)
    set(list_args PROTOS)

    cmake_parse_arguments(
        PARSE_ARGV 0
        PARSED_ARGS
        "${options}"
        "${args}"
        "${list_args}"
    )
    
    if(NOT PARSED_ARGS_TARGET)
        message(FATAL_ERROR "You must provide a target.")
    endif()
    if(NOT PARSED_ARGS_FOLDER_NAME)
        message(FATAL_ERROR "You must provide a folder name.")
    endif()
    if(NOT PARSED_ARGS_PROTOS)
        message(FATAL_ERROR "You must provide a protos.")
    endif()

    protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${PARSED_ARGS_PROTOS})

    add_library(${PARSED_ARGS_TARGET} ${PROTO_HEADER} ${PROTO_SRC} ${PARSED_ARGS_PROTOS})

    # Cheeky copy
    foreach(PROTO_FILE IN LISTS PROTO_HEADER)
        string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}" "" PROTO_NAME "${PROTO_FILE}")

        add_custom_command(TARGET ${PARSED_ARGS_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROTO_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/include/${PARSED_ARGS_FOLDER_NAME}${PROTO_NAME}")
    endforeach()

    target_link_libraries(${PARSED_ARGS_TARGET} PUBLIC ${Protobuf_LIBRARIES})
    target_include_directories(${PARSED_ARGS_TARGET} PUBLIC "${PROTOBUF_INCLUDE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/include")
endfunction()
