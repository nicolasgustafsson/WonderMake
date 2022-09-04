cmake_minimum_required(VERSION 3.19)

list(APPEND SourceFiles
    "include/wondermake-base/Cargo.h"
    "include/wondermake-base/CmdLineArgsSystem.h"
    "include/wondermake-base/Configuration.h"
    "include/wondermake-base/ConfigurationSystem.h"
    "include/wondermake-base/GuidGeneratorSystem.h"
    "include/wondermake-base/JobBase.h"
    "include/wondermake-base/JobGlobal.h"
    "include/wondermake-base/JobRegistry.h"
    "include/wondermake-base/JobSystem.h"
    "include/wondermake-base/Logger.h"
    "include/wondermake-base/LoggerTypes.h"
    "include/wondermake-base/LogTag.h"
    "include/wondermake-base/Process.h"
    "include/wondermake-base/ProcessId.h"
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
    "include/wondermake-base/WmLogTags.h"
    "source/CmdLineArgsSystem.cpp"
    "source/Configuration.cpp"
    "source/ConfigurationSystem.cpp"
    "source/JobGlobal.cpp"
    "source/JobSystem.cpp"
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

set_pedantic_warning_level(WonderMakeBase)
set_whole_archive(WonderMakeBase)
target_include_directories(WonderMakeBase PUBLIC "include")

target_link_libraries(WonderMakeBase WonderMakeUtility)
