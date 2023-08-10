cmake_minimum_required(VERSION 3.19)

list(APPEND IncludeFiles
    "Audio/AudioManager.h"
    "Audio/AudioMixingNodeGraph.h"
    "Audio/AudioNodeGraphResourceFactory.h"
    "Audio/AudioNodeTypes.h"
    "Audio/AudioStructs.h"
    "Audio/SoundEffectNodeGraph.h"
    "Camera/Camera.h"
    "Camera/CameraManager.h"
    "Camera/Display.h"
    "Collision/ColliderDebug.h"
    "Collision/Colliders.h"
    "Collision/CollisionComponent.h"
    "Collision/CollisionFunctionality.h"
    "Collision/CollisionSystem.h"
    "Components/TransformComponent.h"
    "Debugging/DebugLine.h"
    "Debugging/DebugLineDrawer.h"
    "Debugging/DebugLineRenderObject.h"
    "EngineTools/EasingTester.h"
    "EngineTools/EngineTools.h"
    "Functionalities/SpriteRenderingFunctionality.h"
    "Functionalities/TimerFunctionality.h"
    "Functionalities/TransformFunctionality.h"
    "Graphics/RenderNodeGraph/RenderNodeGraph.h"
    "Graphics/RenderNodeGraph/RenderNodeGraphResourceFactory.h"
    "Graphics/RenderNodeGraph/RenderNodeTypes.h"
    "Graphics/Text/TextRenderObject.h"
    "Graphics/RenderCommandProcessor.h"
    "Graphics/Renderer.h"
    "Graphics/RenderHandle.h"
    "Graphics/RenderObject.h"
    "Graphics/ScreenPassRenderObject.h"
    "Graphics/SpriteRenderObject.h"
    "Imgui/Canvas.h"
    "Imgui/NodeGraphGui.h"
    "Input/InputItems.h"
    "Input/InputSystem.h"
    "Message/DispatchableBuffer.h"
    "Message/DispatchRouter.h"
    "Message/Messages.h"
    "Message/MessageSubscriber.h"
    "Message/MessageTypes.h"
    "NodeGraph/SlotCustomization/SlotColors.h"
    "NodeGraph/SlotCustomization/SlotCustomization.md"
    "NodeGraph/SlotCustomization/SlotInputEdits.h"
    "NodeGraph/SlotCustomization/SlotSerialization.h"
    "NodeGraph/CommonNodes.h"
    "NodeGraph/NodeGraph.h"
    "NodeGraph/Nodegraph.md"
    "NodeGraph/NodeTypes.h"
    "Policies/Scheduler.h"
    "Program/Engine.h"
    "Threads/TaskManager.h"
    "Threads/ThreadChecker.h"
    "Utilities/Container/BackendImplementations/ColonyBackend.h"
    "Utilities/Container/BackendImplementations/LinkedListBackend.h"
    "Utilities/Container/BackendImplementations/UnorderedMapBackend.h"
    "Utilities/Container/BackendImplementations/VectorBackend.h"
    "Utilities/Container/Traits/ContainerTraits.h"
    "Utilities/Container/Traits/Implications.h"
    "Utilities/Container/Traits/TraitList.h"
    "Utilities/Container/Container.h"
    "Utilities/Container/ContainerBackend.h"
    "Utilities/Debugging/Debugged.h"
    "Utilities/Easing/EasingFunctions.h"
    "Utilities/TypeTraits/Concepts.h"
    "Utilities/TypeTraits/ContainsType.h"
    "Utilities/TypeTraits/ParameterPack.h"
    "Utilities/TypeTraits/TypeTraits.h"
    "Utilities/BezierCurve.h"
    "Utilities/Id.h"
    "Utilities/Platform.h"
    "Utilities/TimeKeeper.h"
    "Constants.h")

list(APPEND SourceFiles
    "Audio/AudioManager.cpp"
    "Audio/AudioMixingNodeGraph.cpp"
    "Audio/AudioNodeGraphResourceFactory.cpp"
    "Audio/AudioNodeTypes.cpp"
    "Audio/SoundEffectNodeGraph.cpp"
    "Camera/Camera.cpp"
    "Camera/CameraManager.cpp"
    "Camera/Display.cpp"
    "Collision/ColliderDebug.cpp"
    "Collision/CollisionFunctionality.cpp"
    "Collision/CollisionSystem.cpp"
    "Components/Component.cpp"
    "Debugging/DebugLineDrawer.cpp"
    "Debugging/DebugLineRenderObject.cpp"
    "EngineTools/EasingTester.cpp"
    "EngineTools/EngineTools.cpp"
    "Functionalities/SpriteRenderingFunctionality.cpp"
    "Functionalities/TimerFunctionality.cpp"
    "Functionalities/TransformFunctionality.cpp"
    "Graphics/RenderNodeGraph/RenderNodeGraph.cpp"
    "Graphics/RenderNodeGraph/RenderNodeGraphResourceFactory.cpp"
    "Graphics/RenderNodeGraph/RenderNodeTypes.cpp"
    "Graphics/Text/TextRenderObject.cpp"
    "Graphics/RenderCommandProcessor.cpp"
    "Graphics/Renderer.cpp"
    "Graphics/RenderHandle.cpp"
    "Graphics/RenderObject.cpp"
    "Graphics/ScreenPassRenderObject.cpp"
    "Graphics/SpriteRenderObject.cpp"
    "Imgui/Canvas.cpp"
    "Imgui/NodeGraphGui.cpp"
    "Input/InputSystem.cpp"
    "Message/DispatchableBuffer.cpp"
    "Message/DispatchRouter.cpp"
    "Message/MessageSubscriber.cpp"
    "NodeGraph/SlotCustomization/SlotCustomization.md"
    "NodeGraph/NodeGraph.cpp"
    "NodeGraph/NodeTypes.cpp"
    "Policies/Scheduler.cpp"
    "Program/Engine.cpp"
    "Threads/TaskManager.cpp"
    "Utilities/Debugging/Debugged.cpp"
    "Utilities/BezierCurve.cpp"
    "Utilities/Id.cpp"
    "Utilities/Platform.cpp"
    "Utilities/TimeKeeper.cpp"
    "glad.c")

list(APPEND TargetDependencies Glad Glfw Khr Imgui NlohmannJson SoLoud Stb WonderMakeBase WonderMakeUtility WonderMakeIo WonderMakeEngine WonderMakeUi)

if (NOT WonderMake_ImportLibraries)
    set_source_files_properties("glad.c" PROPERTIES LANGUAGE CXX)
    
    add_library(WonderMake STATIC ${IncludeFiles} ${SourceFiles})
    
    set_pedantic_warning_level(WonderMake)
    set_whole_archive(WonderMake PUBLIC)
    target_include_directories(WonderMake PUBLIC ".")
    target_link_libraries(WonderMake ${TargetDependencies})
    
    #Temporary until GLFW include is fixed.
    target_link_libraries(WonderMake "${CMAKE_CURRENT_LIST_DIR}/../../External/glfw-3.3/Lib/glfw3_x64_${CMAKE_BUILD_TYPE}.lib")

    #Temporary until SoLoud include is fixed.
    target_link_libraries(WonderMake "${CMAKE_CURRENT_LIST_DIR}/../../External/SoLoud/Lib/soloud_x64_${CMAKE_BUILD_TYPE}.lib")
else()
    add_library(WonderMake STATIC IMPORTED GLOBAL ${IncludeFiles})
    
    set_whole_archive(WonderMake INTERFACE)
    target_link_libraries(WonderMake INTERFACE ${TargetDependencies})
    target_include_directories(WonderMake INTERFACE ".")

    #Temporary until GLFW include is fixed.
    target_link_libraries(WonderMake INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../../External/glfw-3.3/Lib/glfw3_x64_${CMAKE_BUILD_TYPE}.lib")

    #Temporary until SoLoud include is fixed.
    target_link_libraries(WonderMake INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../../External/SoLoud/Lib/soloud_x64_${CMAKE_BUILD_TYPE}.lib")
endif()

add_compile_definitions("IMGUI_DEFINE_MATH_OPERATORS")
