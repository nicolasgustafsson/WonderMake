cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "Include/WonderMakeBase/CmdLineArgsSystem.h"
    "Include/WonderMakeBase/GuidGeneratorSystem.h"
    "Include/WonderMakeBase/Logger.h"
    "Include/WonderMakeBase/LoggerTypes.h"
    "Include/WonderMakeBase/Process.h"
    "Include/WonderMakeBase/ProcessSystem.h"
    "Include/WonderMakeBase/ScheduleSystem.h"
    "Include/WonderMakeBase/System.h"
    "Include/WonderMakeBase/SystemContainer.h"
    "Include/WonderMakeBase/SystemGlobal.h"
    "Include/WonderMakeBase/SystemId.h"
    "Include/WonderMakeBase/SystemPolicy.h"
    "Include/WonderMakeBase/SystemPtr.h"
    "Include/WonderMakeBase/SystemRegistry.h"
    "Include/WonderMakeBase/SystemTraits.h"
    "Include/WonderMakeBase/WinEventSystem.h"
    "Include/WonderMakeBase/WinPlatformSystem.h"
    "Source/CmdLineArgsSystem.cpp"
    "Source/Logger.cpp"
    "Source/ScheduleSystem.cpp"
    "Source/SystemGlobal.cpp"
    "Source/SystemRegistry.cpp"
    "Source/WinEventSystemImpl.cpp"
    "Source/WinEventSystemImpl.h"
    "Source/WinGuidGeneratorSystem.cpp"
    "Source/WinGuidGeneratorSystem.h"
    "Source/WinLoggerConsoleSystem.cpp"
    "Source/WinLoggerConsoleSystem.h"
    "Source/WinLoggerDebugSystem.cpp"
    "Source/WinLoggerDebugSystem.h"
    "Source/WinPlatformSystemImpl.cpp"
    "Source/WinPlatformSystemImpl.h"
    "Source/WinProcess.cpp"
    "Source/WinProcess.h"
    "Source/WinProcessSystem.cpp"
    "Source/WinProcessSystem.h"
    "Source/WinProgram.cpp"
    "Source/WinProgram.h")

add_library(WonderMakeBase STATIC ${SourceFiles})

set_target_properties(WonderMakeBase PROPERTIES FOLDER "wondermake/lib")

target_include_directories(WonderMakeBase PUBLIC "Include")
if(MSVC)
    target_link_options(WonderMakeBase PUBLIC "/WHOLEARCHIVE:WonderMakeBase")
endif()

target_link_libraries(WonderMakeBase WonderMakeUtility)
