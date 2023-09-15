cmake_minimum_required(VERSION 3.19)

list(APPEND IncludeFiles
    "include/wondermake-ui/AudioResource.h"
    "include/wondermake-ui/DebugSettingsSystem.h"
    "include/wondermake-ui/DebugSystem.h"
    "include/wondermake-ui/EngineUniformBuffer.h"
    "include/wondermake-ui/FileSelectSystem.h"
    "include/wondermake-ui/FontMetrics.h"
    "include/wondermake-ui/FontResource.h"
    "include/wondermake-ui/FontResourceSystem.h"
    "include/wondermake-ui/GlfwFacade.h"
    "include/wondermake-ui/ImguiInclude.h"
    "include/wondermake-ui/ImguiWrapper.h"
    "include/wondermake-ui/OpenGLFacade.h"
    "include/wondermake-ui/RenderSettingsManager.h"
    "include/wondermake-ui/RenderTarget.h"
    "include/wondermake-ui/ShaderParsedData.h"
    "include/wondermake-ui/ShaderProgram.h"
    "include/wondermake-ui/ShaderResource.h"
    "include/wondermake-ui/ShaderResourceSystem.h"
    "include/wondermake-ui/TextureResource.h"
    "include/wondermake-ui/TextureResourceSystem.h"
    "include/wondermake-ui/UniformBuffer.h"
    "include/wondermake-ui/VertexAttributes.h"
    "include/wondermake-ui/VertexBuffer.h"
    "include/wondermake-ui/VertexBufferArray.h"
    "include/wondermake-ui/VertexTypes.h"
    "include/wondermake-ui/Window.h")

list(APPEND SourceFiles
    "source/AudioResourceFactory.cpp"
    "source/AudioResourceFactory.h"
    "source/ConfigurationUi.cpp"
    "source/ConfigurationUi.h"
    "source/ConfigurationView.cpp"
    "source/ConfigurationView.h"
    "source/DebugSettingsSystem.cpp"
    "source/DebugSystem.cpp"
    "source/EngineUniformBuffer.cpp"
    "source/FileSelectSystem.cpp"
    "source/FontResourceFactory.cpp"
    "source/FontResourceFactory.h"
    "source/FontResourceSystem.cpp"
    "source/GlfwFacade.cpp"
    "source/ImGuiLogger.cpp"
    "source/ImGuiLogger.h"
    "source/ImguiWrapper.cpp"
    "source/JsonInspector.cpp"
    "source/JsonInspector.h"
    "source/JsonExtensions.h"
    "source/JsonUtility.cpp"
    "source/JsonUtility.h"
    "source/OpenGLFacade.cpp"
    "source/RenderSettingsManager.cpp"
    "source/RenderTarget.cpp"
    "source/ShaderResourceFactory.h"
    "source/ShaderResourceFactoryFragment.cpp"
    "source/ShaderResourceFactoryGeometry.cpp"
    "source/ShaderResourceFactoryVertex.cpp"
    "source/ShaderResourceSystem.cpp"
    "source/ShaderParsedData.cpp"
    "source/ShaderParser.cpp"
    "source/ShaderParser.h"
    "source/StbImplementation.cpp"
    "source/SystemRegistryView.cpp"
    "source/SystemRegistryView.h"
    "source/TextureResourceFactory.cpp"
    "source/TextureResourceFactory.h"
    "source/TextureResourceSystem.cpp"
    "source/Window.cpp")

list(APPEND TargetDependencies WonderMakeEngine WonderMakeIo WonderMakeBase WonderMakeUtility magic_enum::magic_enum Glad Glfw Khr Imgui Stb SoLoud)

if (NOT WonderMake_ImportLibraries)
    add_library(WonderMakeUi STATIC ${IncludeFiles} ${SourceFiles})
    
    set_pedantic_warning_level(WonderMakeUi)
    set_whole_archive(WonderMakeUi PUBLIC)
    target_include_directories(WonderMakeUi PUBLIC "include")
    target_link_libraries(WonderMakeUi ${TargetDependencies})
else()
    add_library(WonderMakeUi STATIC IMPORTED GLOBAL ${IncludeFiles})
    
    set_whole_archive(WonderMakeUi INTERFACE)
    target_include_directories(WonderMakeUi INTERFACE "include")
    target_link_libraries(WonderMakeUi INTERFACE ${TargetDependencies})
endif()
