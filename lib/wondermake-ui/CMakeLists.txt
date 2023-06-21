cmake_minimum_required(VERSION 3.19)

list(APPEND IncludeFiles
    "include/wondermake-ui/EngineUniformBuffer.h"
    "include/wondermake-ui/GlfwFacade.h"
    "include/wondermake-ui/ImguiWrapper.h"
    "include/wondermake-ui/OpenGLFacade.h"
    "include/wondermake-ui/RenderSettingsManager.h"
    "include/wondermake-ui/RenderTarget.h"
    "include/wondermake-ui/ShaderParser.h"
    "include/wondermake-ui/UniformBuffer.h"
    "include/wondermake-ui/VertexAttributes.h"
    "include/wondermake-ui/VertexBuffer.h"
    "include/wondermake-ui/VertexBufferArray.h"
    "include/wondermake-ui/VertexTypes.h"
    "include/wondermake-ui/Window.h")

list(APPEND SourceFiles
    "source/EngineUniformBuffer.cpp"
    "source/GlfwFacade.cpp"
    "source/OpenGLFacade.cpp"
    "source/RenderSettingsManager.cpp"
    "source/RenderTarget.cpp"
    "source/ShaderParser.cpp"
    "source/Window.cpp")

list(APPEND TargetDependencies WonderMakeEngine WonderMakeIo WonderMakeBase WonderMakeUtility magic_enum::magic_enum Glad Glfw Khr)

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
