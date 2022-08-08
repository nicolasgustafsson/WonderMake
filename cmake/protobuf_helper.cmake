
function(add_protobuf_library ARG_TARGET ARG_FOLDER_NAME ARG_PROTO_LIST)
    protobuf_generate_cpp(PROTO_SRC PROTO_HEADER ${ARG_PROTO_LIST})

    add_library(${ARG_TARGET} ${PROTO_HEADER} ${PROTO_SRC} ${ARG_PROTO_LIST})

    # Cheeky copy
    foreach(PROTO_FILE IN LISTS PROTO_HEADER)
        string(REPLACE "${CMAKE_CURRENT_BINARY_DIR}" "" PROTO_NAME "${PROTO_FILE}")

        add_custom_command(TARGET ${ARG_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${PROTO_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/include/${ARG_FOLDER_NAME}${PROTO_NAME}")
    endforeach()

    target_link_libraries(${ARG_TARGET} PUBLIC ${Protobuf_LIBRARIES})
    target_include_directories(${ARG_TARGET} PUBLIC "${PROTOBUF_INCLUDE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/include")
endfunction()
