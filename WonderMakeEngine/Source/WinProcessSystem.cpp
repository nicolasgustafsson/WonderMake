#pragma once

#include "WinProcessSystem.h"

#include "WinProcess.h"

#include "WonderMakeEngine/SystemGlobal.h"
#include "WonderMakeEngine/WinEventSystem.h"
#include "WonderMakeEngine/WinPlatformSystem.h"

#include "WonderMakeBase/Bindable.h"

REGISTER_SYSTEM_MASKED(WinProcessSystem, ProcessSystem);

Result<ProcessSystem::EStartError, std::shared_ptr<Process>> WinProcessSystem::StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine)
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
            return { EStartError::FileNotFound, err };

        return { EStartError::InternalError, err };
    }

    auto process = std::make_shared<WinProcess>(Get<WinEventSystem>(), Get<WinPlatformSystem>(), proccessInformation.hProcess, proccessInformation.hThread);

    process->Initialize();

    return { std::move(process) };
}
