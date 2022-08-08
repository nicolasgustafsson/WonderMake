
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
