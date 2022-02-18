cmake_minimum_required(VERSION 3.10)

list(APPEND SourceFiles
    "include/wondermake-base/jobs/JobBase.h"
    "include/wondermake-base/jobs/JobGlobal.h"
    "include/wondermake-base/jobs/JobRegistry.h"
    "include/wondermake-base/jobs/JobSystem.h"
    "include/wondermake-base/Cargo.h"
    "include/wondermake-base/CmdLineArgsSystem.h"
    "include/wondermake-base/GuidGeneratorSystem.h"
    "include/wondermake-base/Logger.h"
    "include/wondermake-base/LoggerTypes.h"
    "include/wondermake-base/Process.h"
    "include/wondermake-base/ProcessSystem.h"
    "include/wondermake-base/ScheduleSystem.h"
    "include/wondermake-base/System.h"
    "include/wondermake-base/SystemContainer.h"
    "include/wondermake-base/SystemGlobal.h"
    "include/wondermake-base/SystemId.h"
    "include/wondermake-base/SystemPolicy.h"
    "include/wondermake-base/SystemPtr.h"
    "include/wondermake-base/SystemRegistry.h"
    "include/wondermake-base/SystemTraits.h"
    "include/wondermake-base/WinEventSystem.h"
    "include/wondermake-base/WinPlatformSystem.h"
    "source/Jobs/JobGlobal.cpp"
    "source/Jobs/JobSystem.cpp"
    "source/CmdLineArgsSystem.cpp"
    "source/Logger.cpp"
    "source/ScheduleSystem.cpp"
    "source/SystemGlobal.cpp"
    "source/SystemRegistry.cpp"
    "source/WinEventSystemImpl.cpp"
    "source/WinEventSystemImpl.h"
    "source/WinGuidGeneratorSystem.cpp"
    "source/WinGuidGeneratorSystem.h"
    "source/WinLoggerConsoleSystem.cpp"
    "source/WinLoggerConsoleSystem.h"
    "source/WinLoggerDebugSystem.cpp"
    "source/WinLoggerDebugSystem.h"
    "source/WinPlatformSystemImpl.cpp"
    "source/WinPlatformSystemImpl.h"
    "source/WinProcess.cpp"
    "source/WinProcess.h"
    "source/WinProcessSystem.cpp"
    "source/WinProcessSystem.h"
    "source/WinProgram.cpp"
    "source/WinProgram.h")

add_library(WonderMakeBase STATIC ${SourceFiles})

target_include_directories(WonderMakeBase PUBLIC "include")
if(MSVC)
    target_link_options(WonderMakeBase PUBLIC "/WHOLEARCHIVE:WonderMakeBase")
endif()

target_link_libraries(WonderMakeBase WonderMakeUtility)
