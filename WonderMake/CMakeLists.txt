set(PROJECT_NAME WonderMake)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "Universe/UniverseDenizen.cpp"
    "Universe/UniverseDenizen.h"
    "Universe/UniverseHelper.cpp"
    "Universe/UniverseHelper.h"
    "Universe/UniverseManagerSystem.cpp"
    "Universe/UniverseManagerSystem.h"
    "Universe/UniverseSystem.cpp"
    "Universe/UniverseSystem.h"
    "Audio/AudioFile.cpp"
    "Audio/AudioFile.h"
    "Audio/AudioManager.cpp"
    "Audio/AudioManager.h"
    "Audio/AudioMixingNodeGraph.cpp"
    "Audio/AudioMixingNodeGraph.h"
    "Audio/AudioNodeTypes.cpp"
    "Audio/AudioNodeTypes.h"
    "Audio/AudioStructs.h"
    "Audio/SoundEffectNodeGraph.cpp"
    "Audio/SoundEffectNodeGraph.h"
    "Camera/Camera.cpp"
    "Camera/Camera.h"
    "Camera/CameraManager.cpp"
    "Camera/CameraManager.h"
    "Camera/Display.cpp"
    "Camera/Display.h"
    "Cheats/CheatManager.cpp"
    "Cheats/CheatManager.h"
    "Collision/ColliderDebug.cpp"
    "Collision/ColliderDebug.h"
    "Collision/Colliders.h"
    "Collision/CollisionComponent.cpp"
    "Collision/CollisionComponent.h"
    "Collision/CollisionFunctionality.cpp"
    "Collision/CollisionFunctionality.h"
    "Collision/CollisionSystem.cpp"
    "Collision/CollisionSystem.h"
    "CommonExtensions/AttachedFunctionality.cpp"
    "CommonExtensions/AttachedFunctionality.h"
    "CommonExtensions/Camera/CameraFunctionality.cpp"
    "CommonExtensions/Camera/CameraFunctionality.h"
    "CommonExtensions/Levels/ObjectContainerFunctionality.cpp"
    "CommonExtensions/Levels/ObjectContainerFunctionality.h"
    "CommonExtensions/Levels/SubobjectFunctionality.cpp"
    "CommonExtensions/Levels/SubobjectFunctionality.h"
    "CommonExtensions/SpinnerFunctionality.cpp"
    "CommonExtensions/SpinnerFunctionality.h"
    "CommonExtensions/TimeToLiveFunctionality.cpp"
    "CommonExtensions/TimeToLiveFunctionality.h"
    "Components/Component.cpp"
    "Components/Component.h"
    "Components/ComponentSystem.h"
    "Components/SpriteComponent.h"
    "Components/TransformComponent.h"
    "Constants.h"
    "Debugging/DebugLine.h"
    "Debugging/DebugLineDrawer.cpp"
    "Debugging/DebugLineDrawer.h"
    "Debugging/DebugLineRenderObject.cpp"
    "Debugging/DebugLineRenderObject.h"
    "Debugging/DebugSettingsSystem.cpp"
    "Debugging/DebugSettingsSystem.h"
    "EngineTools/EasingTester.cpp"
    "EngineTools/EasingTester.h"
    "EngineTools/EngineTools.cpp"
    "EngineTools/EngineTools.h"
    "FileSystem/FileWatcher.cpp"
    "FileSystem/FileWatcher.h"
    "Functionalities/BaseFunctionality.h"
    "Functionalities/Functionality.h"
    "Functionalities/FunctionalitySystem.h"
    "Functionalities/ObjectInterfaceFunctionality.cpp"
    "Functionalities/ObjectInterfaceFunctionality.h"
    "Functionalities/SpriteRenderingFunctionality.cpp"
    "Functionalities/SpriteRenderingFunctionality.h"
    "Functionalities/TimerFunctionality.cpp"
    "Functionalities/TimerFunctionality.h"
    "Functionalities/TransformFunctionality.cpp"
    "Functionalities/TransformFunctionality.h"
    "glad.c"
    "Graphics/EngineUniformBuffer.cpp"
    "Graphics/EngineUniformBuffer.h"
    "Graphics/OpenGLFacade.cpp"
    "Graphics/OpenGLFacade.h"
    "Graphics/RectangleRenderObject.cpp"
    "Graphics/RectangleRenderObject.h"
    "Graphics/RenderCommandProcessor.cpp"
    "Graphics/RenderCommandProcessor.h"
    "Graphics/Renderer.cpp"
    "Graphics/Renderer.h"
    "Graphics/RenderHandle.cpp"
    "Graphics/RenderHandle.h"
    "Graphics/RenderNodeGraph/RenderNodeGraph.cpp"
    "Graphics/RenderNodeGraph/RenderNodeGraph.h"
    "Graphics/RenderNodeGraph/RenderNodeTypes.cpp"
    "Graphics/RenderNodeGraph/RenderNodeTypes.h"
    "Graphics/RenderObject.cpp"
    "Graphics/RenderObject.h"
    "Graphics/RenderSettingsManager.cpp"
    "Graphics/RenderSettingsManager.h"
    "Graphics/RenderTarget.cpp"
    "Graphics/RenderTarget.h"
    "Graphics/ScreenPassRenderObject.cpp"
    "Graphics/ScreenPassRenderObject.h"
    "Graphics/Shader.cpp"
    "Graphics/Shader.h"
    "Graphics/ShaderParser.cpp"
    "Graphics/ShaderParser.h"
    "Graphics/ShaderProgram.cpp"
    "Graphics/ShaderProgram.h"
    "Graphics/SpriteBatchRenderObject.cpp"
    "Graphics/SpriteBatchRenderObject.h"
    "Graphics/SpriteRenderObject.cpp"
    "Graphics/SpriteRenderObject.h"
    "Graphics/Text/Font.cpp"
    "Graphics/Text/Font.h"
    "Graphics/Text/FontMetrics.h"
    "Graphics/Text/TextRenderObject.cpp"
    "Graphics/Text/TextRenderObject.h"
    "Graphics/Texture.cpp"
    "Graphics/Texture.h"
    "Graphics/UniformBuffer.h"
    "Graphics/VertexAttributes.h"
    "Graphics/VertexBuffer.h"
    "Graphics/VertexBufferArray.h"
    "Graphics/VertexTypes.h"
    "Imgui/AssetBrowser.cpp"
    "Imgui/AssetBrowser.h"
    "Imgui/Canvas.cpp"
    "Imgui/Canvas.h"
    "Imgui/DockSpace.cpp"
    "Imgui/DockSpace.h"
    "Imgui/FileSelector.cpp"
    "Imgui/FileSelector.h"
    "Imgui/ImguiInclude.h"
    "Imgui/JsonInspector.cpp"
    "Imgui/JsonInspector.h"
    "Imgui/NodeGraphGui.cpp"
    "Imgui/NodeGraphGui.h"
    "Input/InputItems.h"
    "Input/InputSystem.cpp"
    "Input/InputSystem.h"
    "Job/Cargo.h"
    "Job/Job.h"
    "Job/JobBase.cpp"
    "Job/JobBase.h"
    "Job/JobFuture.h"
    "Job/JobOutput.h"
    "Job/JobPromise.h"
    "Job/JobSystem.cpp"
    "Job/JobSystem.h"
    "LineListRenderObject.cpp"
    "LineListRenderObject.h"
    "Logging/ConsoleLogger.cpp"
    "Logging/ConsoleLogger.h"
    "Logging/FileLogger.cpp"
    "Logging/FileLogger.h"
    "Logging/ImGuiLogger.cpp"
    "Logging/ImGuiLogger.h"
    "Logging/LogMessageTags.h"
    "Message/DispatchableBuffer.cpp"
    "Message/DispatchableBuffer.h"
    "Message/DispatchRouter.cpp"
    "Message/DispatchRouter.h"
    "Message/Messages.h"
    "Message/MessageSubscriber.cpp"
    "Message/MessageSubscriber.h"
    "Message/MessageTypes.h"
    "NodeGraph/CommonNodes.h"
    "NodeGraph/NodeGraph.cpp"
    "NodeGraph/NodeGraph.h"
    "NodeGraph/Nodegraph.md"
    "NodeGraph/NodeTypes.cpp"
    "NodeGraph/NodeTypes.h"
    "NodeGraph/SlotCustomization/SlotColors.h"
    "NodeGraph/SlotCustomization/SlotCustomization.md"
    "NodeGraph/SlotCustomization/SlotInputEdits.h"
    "NodeGraph/SlotCustomization/SlotSerialization.h"
    "Object/CreateObjectJob.h"
    "Object/DependencyDestructor.cpp"
    "Object/DependencyDestructor.h"
    "Object/Object.cpp"
    "Object/Object.h"
    "pch.cpp"
    "pch.h"
    "Policies/Policy.h"
    "Policies/Scheduler.cpp"
    "Policies/Scheduler.h"
    "Policies/SystemId.h"
    "Program/Engine.cpp"
    "Program/Engine.h"
    "Program/GlfwFacade.cpp"
    "Program/GlfwFacade.h"
    "Program/ImguiWrapper.cpp"
    "Program/ImguiWrapper.h"
    "Program/Program.cpp"
    "Program/Program.h"
    "Program/Window.cpp"
    "Program/Window.h"
    "Randomizer/Randomizer.cpp"
    "Randomizer/Randomizer.h"
    "Resources/Asset.h"
    "Resources/AssetDatabase.h"
    "Resources/AssetLink.h"
    "Resources/CreateResource.h"
    "Resources/Resource.h"
    "Resources/ResourceProxy.h"
    "Resources/ResourceSystem.h"
    "Scheduling/ScheduleSystem.cpp"
    "Scheduling/ScheduleSystem.h"
    "System/System.h"
    "System/SystemContainer.cpp"
    "System/SystemContainer.h"
    "System/SystemPtr.h"
    "Threads/Routine.cpp"
    "Threads/Routine.h"
    "Threads/TaskManager.cpp"
    "Threads/TaskManager.h"
    "Threads/ThreadChecker.h"
    "TweakableValues/TweakableValueDatabase.cpp"
    "TweakableValues/TweakableValueDatabase.h"
    "Typedefs.h"
    "Utilities/Alias.h"
    "Utilities/AnimatedValue.cpp"
    "Utilities/AnimatedValue.h"
    "Utilities/AutoRegister.h"
    "Utilities/Geometry/BezierCurve.cpp"
    "Utilities/Geometry/BezierCurve.h"
    "Utilities/Color.cpp"
    "Utilities/Color.h"
    "Utilities/Container/BackendImplementations/ColonyBackend.h"
    "Utilities/Container/BackendImplementations/LinkedListBackend.h"
    "Utilities/Container/BackendImplementations/UnorderedMapBackend.h"
    "Utilities/Container/BackendImplementations/VectorBackend.h"
    "Utilities/Container/Container.h"
    "Utilities/Container/ContainerBackend.h"
    "Utilities/Container/Traits/ContainerTraits.h"
    "Utilities/Container/Traits/Implications.h"
    "Utilities/Container/Traits/TraitList.h"
    "Utilities/Debugging/Debugged.cpp"
    "Utilities/Debugging/Debugged.h"
    "Utilities/DependencyInjector.cpp"
    "Utilities/DependencyInjector.h"
    "Utilities/Easing/EasingFunctions.h"
    "Utilities/Geometry/Geometry.h"
    "Utilities/Id.cpp"
    "Utilities/Id.h"
    "Utilities/Json/JsonExtensions.h"
    "Utilities/Json/JsonUtility.cpp"
    "Utilities/Json/JsonUtility.h"
    "Utilities/Geometry/Line.h"
    "Utilities/Matrix.h"
    "Utilities/Platform.cpp"
    "Utilities/Platform.h"
    "Utilities/plf_colony.h"
    "Utilities/RestrictTypes.h"
    "Utilities/Result.h"
    "Utilities/Rotation.h"
    "Utilities/Singleton.h"
    "Utilities/Spline.cpp"
    "Utilities/Spline.h"
    "Utilities/StateMachine.cpp"
    "Utilities/StateMachine.h"
    "Utilities/StaticRegistry/FixedString.h"
    "Utilities/StaticRegistry/StaticRegistry.cpp"
    "Utilities/StaticRegistry/StaticRegistry.h"
    "Utilities/Stopwatch.h"
    "Utilities/TimeKeeper.cpp"
    "Utilities/TimeKeeper.h"
    "Utilities/TypeTraits/Concepts.h"
    "Utilities/TypeTraits/ContainsType.h"
    "Utilities/TypeTraits/ParameterPack.h"
    "Utilities/TypeTraits/TypeString.h"
    "Utilities/TypeTraits/TypeTraits.h"
    "Utilities/UniqueFunction.h"
    "Utilities/UniqueValue.h"
    "Utilities/Utility.h"
    "Utilities/Vector.h"
    "Utilities/WmMath.cpp"
    "Utilities/WmMath.h"
    "Object/BaseObject.cpp"
    "Object/BaseObject.h"
    "Utilities/Range.h"
    "Utilities/Geometry/Intersections.h"
    "Utilities/Geometry/Rectangle.h" Utilities/Geometry/Circle.h Utilities/ToString.h Debugging/CommandPalette.cpp Debugging/CommandPalette.h)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE WonderMake)


################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../External/glad/Inc/;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../External/Imgui/Inc/;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../External/json-3.5.0/Inc/;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../External/SoLoud/Inc/;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../External/glfw-3.3/Inc/;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../External/stb/Inc/;"
)


link_directories(${PROJECT_NAME} PUBLIC
"${CMAKE_CURRENT_SOURCE_DIR}/;"
"${CMAKE_CURRENT_SOURCE_DIR}/../Fletcherette;"
"${CMAKE_CURRENT_SOURCE_DIR}/../WonderMake;"
"${CMAKE_CURRENT_SOURCE_DIR}/../External/glfw-3.3/Lib/;"
"${CMAKE_CURRENT_SOURCE_DIR}/../External/stb/Lib/;"
"${CMAKE_CURRENT_SOURCE_DIR}/../External/glad/Lib/;"
"${CMAKE_CURRENT_SOURCE_DIR}/../External/Imgui/Lib/;"
"${CMAKE_CURRENT_SOURCE_DIR}/../External/json-3.5.0/Lib/;"
"${CMAKE_CURRENT_SOURCE_DIR}/../External/SoLoud/Lib/;"
)


target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

#add_link_options(--whole-archive)
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
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../Lib/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../Lib/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../Lib/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../Lib/"
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
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_MBCS;"
        "_ITERATOR_DEBUG_LEVEL=0;"
        "CONFIGURATION_$<CONFIG>;"
        "IMGUI_DEFINE_MATH_OPERATORS;"
        ""
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_MBCS;"
        "_ITERATOR_DEBUG_LEVEL=0;"
        "CONFIGURATION_$<CONFIG>;"
        "IMGUI_DEFINE_MATH_OPERATORS;"
        ""
    )
endif()


################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
        Imgui
)


# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw3
    pthread
    dl
    Imgui
    soloud
    asound
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

