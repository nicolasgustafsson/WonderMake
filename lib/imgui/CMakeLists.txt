cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "Include/Imgui/imconfig.h"
    "Include/Imgui/imgui.h"
    "Include/Imgui/imgui_impl_glfw.h"
    "Include/Imgui/imgui_impl_opengl3.h"
    "Include/Imgui/imgui_internal.h"
    "Include/Imgui/imgui_stdlib.h"
    "Include/Imgui/imstb_rectpack.h"
    "Include/Imgui/imstb_textedit.h"
    "Include/Imgui/imstb_truetype.h"
    "Source/Imgui.cpp"
    "Source/imgui_demo.cpp"
    "Source/imgui_draw.cpp"
    "Source/imgui_impl_glfw.cpp"
    "Source/imgui_impl_opengl3.cpp"
    "Source/imgui_stdlib.cpp"
    "Source/imgui_tables.cpp"
    "Source/imgui_widgets.cpp"
    "Source/stdafx.cpp"
    "Source/stdafx.h")

add_library(Imgui STATIC ${SourceFiles})

target_include_directories(Imgui
    PUBLIC
        "Include"
    PRIVATE
        "Include/Imgui")

target_link_libraries(Imgui Glad Glfw Khr)

add_compile_definitions("IMGUI_DEFINE_MATH_OPERATORS")
