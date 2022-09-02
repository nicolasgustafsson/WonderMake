
function(get_buildsystem_targets ARG_OUT_BUILDSYSTEM_TARGETS ARG_DIRECTORY)
    set(RESULT_TARGETS)

    get_directory_property(SUBDIRECTORIES DIRECTORY "${ARG_DIRECTORY}" SUBDIRECTORIES)

    foreach(SUBDIR ${SUBDIRECTORIES})
        get_buildsystem_targets(SUBDIR_TARGETS "${SUBDIR}")

        list(APPEND RESULT_TARGETS ${SUBDIR_TARGETS})
    endforeach()
    
    get_directory_property(BUILDSYSTEM_TARGETS DIRECTORY "${ARG_DIRECTORY}" BUILDSYSTEM_TARGETS)

    foreach(TARGET ${BUILDSYSTEM_TARGETS})
        list(APPEND RESULT_TARGETS ${TARGET})
    endforeach()

    set(${ARG_OUT_BUILDSYSTEM_TARGETS} ${RESULT_TARGETS} PARENT_SCOPE)
endfunction()

function(set_buildsystem_targets_folder ARG_DIRECTORY ARG_FOLDER)
    get_buildsystem_targets(DIRECTORY_TARGETS ${ARG_DIRECTORY})
    
    foreach(DIRECTORY_TARGET IN LISTS DIRECTORY_TARGETS)
        get_target_property(TARGET_FOLDER ${DIRECTORY_TARGET} FOLDER)

        if("${TARGET_FOLDER}" STREQUAL "TARGET_FOLDER-NOTFOUND")
            set(TARGET_FOLDER "")
        endif()

        set_target_properties(${DIRECTORY_TARGET} PROPERTIES FOLDER "${ARG_FOLDER}/${TARGET_FOLDER}")
    endforeach()
endfunction()

function(set_whole_archive ARG_TARGET)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_link_options(${ARG_TARGET} PUBLIC "/WHOLEARCHIVE:WonderMakeEngine")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${ARG_TARGET} PUBLIC "--whole-archive")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(SEND_ERROR "Flag for setting whole-archive is currently not set for Clang.")
    else()
        message(SEND_ERROR "Unknown c++ compiler, unable to set flag for whole-archive.")
    endif()
endfunction()

function(set_pedantic_warning_level ARG_TARGET)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${ARG_TARGET} PRIVATE /W4 /WX)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${ARG_TARGET} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(SEND_ERROR "Flag for setting pedantic warning level is currently not set for Clang.")
    else()
        message(SEND_ERROR "Unknown c++ compiler, unable to set flag for pedantic warning level.")
    endif()
endfunction()
