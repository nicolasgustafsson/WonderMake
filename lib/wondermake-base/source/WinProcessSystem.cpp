#pragma once

#include "WinProcessSystem.h"

#include "WinProcess.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinEventSystem.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/Bindable.h"

REGISTER_SYSTEM_MASKED(WinProcessSystem, ProcessSystem);

ProcessId WinProcessSystem::GetCurrentProcessId()
{
    auto&& winPlatform = Get<WinPlatformSystem>();

    return winPlatform.GetProcessId(winPlatform.GetCurrentProcess());
}

Result<std::shared_ptr<Process>, ProcessSystem::SStartError> WinProcessSystem::StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine)
{
    auto&& winPlatform = Get<WinPlatformSystem>();

    std::wstring commandLineWithApplicationPath = L'"' + aApplicationPath.wstring() + L"\" " + aCommandLine;
    std::wstring applicationPath = aApplicationPath.wstring();

    STARTUPINFOW startupInfo = {};
    PROCESS_INFORMATION proccessInformation = {};

    ZeroMemory(&startupInfo, sizeof(startupInfo));

    startupInfo.cb = sizeof(startupInfo);

    const       LPCWSTR                 lpApplicationName       = applicationPath.c_str();
    const       LPWSTR                  lpCommandLine           = commandLineWithApplicationPath.data();
    constexpr   LPSECURITY_ATTRIBUTES   lpProcessAttributes     = NULL;
    constexpr   LPSECURITY_ATTRIBUTES   lpThreadAttributes      = NULL;
    constexpr   BOOL                    bInheritHandles         = FALSE;
    constexpr   DWORD                   dwCreationFlags         = 0;
    constexpr   LPVOID                  lpEnvironment           = NULL;
    constexpr   LPCWSTR                 lpCurrentDirectory      = NULL;
    const       LPSTARTUPINFOW          lpStartupInfo           = &startupInfo;
    const       LPPROCESS_INFORMATION   lpProcessInformation    = &proccessInformation;

    const BOOL result = winPlatform.CreateProcessW(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);

    if (!result)
    {
        const DWORD err = winPlatform.GetLastError();

        if (err == ERROR_FILE_NOT_FOUND)
            return Err(SStartError{ EStartError::FileNotFound, err });

        return Err(SStartError{ EStartError::InternalError, err });
    }

    auto process = std::make_shared<WinProcess>(GetExecutor(), Get<WinEventSystem>(), Get<WinPlatformSystem>(), proccessInformation.hProcess, proccessInformation.hThread);

    process->Initialize();

    return Ok(std::move(process));
}
