set(PROJECT_NAME Game)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Actions/Action.h"
    "Actions/ActionFunctionality.cpp"
    "Actions/ActionFunctionality.h"
    "Character/Buffs/Buff.cpp"
    "Character/Buffs/Buff.h"
    "Character/Buffs/BuffDesign.md"
    "Character/Buffs/BuffProperties.cpp"
    "Character/Buffs/BuffProperties.h"
    "Character/Buffs/CharacterBuffsFunctionality.cpp"
    "Character/Buffs/CharacterBuffsFunctionality.h"
    "Character/Buffs/LifetimeBuffProperty/BuffLifetimeProperty.cpp"
    "Character/Buffs/LifetimeBuffProperty/BuffLifetimeProperty.h"
    "Character/Buffs/StatChangeProperty/BuffStatChangeProperty.cpp"
    "Character/Buffs/StatChangeProperty/BuffStatChangeProperty.h"
    "Character/Buffs/TimedTickBuffProperty/BuffTimedTickProperty.cpp"
    "Character/Buffs/TimedTickBuffProperty/BuffTimedTickProperty.h"
    "Character/CharacterFunctionality.cpp"
    "Character/CharacterFunctionality.h"
    "Character/Effects/CharacterEffect.cpp"
    "Character/Effects/CharacterEffect.h"
    "Character/Effects/CharacterEffectPhilosophy.md"
    "Character/Stats/CharacterStatsFunctionality.cpp"
    "Character/Stats/CharacterStatsFunctionality.h"
    "Designers/BuffDesigner/BuffDesigner.cpp"
    "Designers/BuffDesigner/BuffDesigner.h"
    "Designers/BuffDesigner/BuffDesignerPhilosophy.md"
    "Designers/DesignerPhilosophy.md"
    "Designers/EffectDesigner/EffectDesigner.cpp"
    "Designers/EffectDesigner/EffectDesigner.h"
    "Designers/EffectDesigner/EffectDesignerPhilosophy.md"
    "Designers/LevelDesigner/Geometry/Polygon.cpp"
    "Designers/LevelDesigner/Geometry/Polygon.h"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonFloatingPointOperator.cpp"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonFloatingPointOperator.h"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonOperators.h"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.cpp"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonPointOperator.h"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonSideOperator.cpp"
    "Designers/LevelDesigner/Geometry/PolygonOperators/PolygonSideOperator.h"
    "Designers/LevelDesigner/LevelDesigner.cpp"
    "Designers/LevelDesigner/LevelDesigner.h"
    "Designers/MeleeWeaponDesigner/MeleeWeaponDesigner.cpp"
    "Designers/MeleeWeaponDesigner/MeleeWeaponDesigner.h"
    "Enemy/EnemyActions/PunchAction.cpp"
    "Enemy/EnemyActions/PunchAction.h"
    "Enemy/EnemyControllerComponent.h"
    "Enemy/EnemyControllerFunctionality.cpp"
    "Enemy/EnemyControllerFunctionality.h"
    "Enemy/TargetFunctionality.cpp"
    "Enemy/TargetFunctionality.h"
    "Game.cpp"
    "Game.h"
    "GameWorld.cpp"
    "GameWorld.h"
    "HitShapes/HitShapeFunctionality.cpp"
    "HitShapes/HitShapeFunctionality.h"
    "HitShapes/HitShapeRenderObject.cpp"
    "HitShapes/HitShapeRenderObject.h"
    "HitShapes/Hitshapes.md"
    "HitShapes/HitShapeSpawnerFunctionality.cpp"
    "HitShapes/HitShapeSpawnerFunctionality.h"
    "Levels/BuffGiver/BuffGiverFunctionality.cpp"
    "Levels/BuffGiver/BuffGiverFunctionality.h"
    "Levels/BuffGiver/BuffGiverRenderObject.cpp"
    "Levels/BuffGiver/BuffGiverRenderObject.h"
    "Levels/LevelPortalFunctionality.cpp"
    "Levels/LevelPortalFunctionality.h"
    "Movement/DefaultMovementFunctionality.cpp"
    "Movement/DefaultMovementFunctionality.h"
    "Movement/MovementInputFunctionality.cpp"
    "Movement/MovementInputFunctionality.h"
    "Physics/Navmesh/Navmesh.cpp"
    "Physics/Navmesh/Navmesh.h"
    "Physics/Navmesh/NavmeshFunctionality.cpp"
    "Physics/Navmesh/NavmeshFunctionality.h"
    "Physics/Navmesh/NavmeshRenderObject.cpp"
    "Physics/Navmesh/NavmeshRenderObject.h"
    "Physics/StaticGeometryFunctionality.cpp"
    "Physics/StaticGeometryFunctionality.h"
    "Player/PlayerControllerFunctionality.cpp"
    "Player/PlayerControllerFunctionality.h"
    "Rendering/Vision/VisionFunctionality.cpp"
    "Rendering/Vision/VisionFunctionality.h"
    "Rendering/Vision/VisionRenderObject.cpp"
    "Rendering/Vision/VisionRenderObject.h"
    "Utility/Palette.h"
    "UtilityFunctionalities/FactionFunctionality.cpp"
    "UtilityFunctionalities/FactionFunctionality.h"
    "UtilityFunctionalities/LineOfSightFunctionality.cpp"
    "UtilityFunctionalities/LineOfSightFunctionality.h"
    "Weapons/MeleeWeapon.cpp"
    "Weapons/MeleeWeapon.h"
    "Weapons/MeleeWeaponUserFunctionality.cpp"
    "Weapons/MeleeWeaponUserFunctionality.h"
    "Weapons/WeaponProperty/WeaponProperty.cpp"
    "Weapons/WeaponProperty/WeaponProperty.h"
    "Weapons/WeaponSwing.h"
    "Weapons/WeaponSwingHitShapeAction.cpp"
    "Weapons/WeaponSwingHitShapeAction.h"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

#target_precompile_headers(${PROJECT_NAME} PRIVATE
#    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
#)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Game)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
        TARGET_NAME_RELEASE "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
        TARGET_NAME_RELEASE "${PROJECT_NAME}_${CMAKE_VS_PLATFORM_NAME}_$<CONFIG>"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Lib/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Lib/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Lib/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Lib/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/."
)

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /MP;
            /JMC;
            /std:c++latest;
            /Gm-;
            /sdl;
            /W4;
            /WX;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /w44062;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /MP;
            /JMC;
            /std:c++latest;
            /Gm-;
            /sdl;
            /W4;
            /WX;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /w44062;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    WonderMake
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3;"
        "imgui"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glfw3;"
        "imgui"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

