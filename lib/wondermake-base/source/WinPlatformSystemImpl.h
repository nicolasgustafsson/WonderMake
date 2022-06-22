#pragma once

#include "wondermake-base/WinPlatformSystem.h"

class WinPlatformSystemImpl
	: public SystemSub<
		Policy::Set<>,
		STrait::Set<
			STFoundational,
			STThreadsafe,
			STSingleton>>
	, public WinPlatformSystem
{
public:
	DWORD GetLastError() override;

	DWORD WaitForMultipleObjects(
		DWORD nCount,
		const HANDLE* lpHandles,
		BOOL bWaitAll,
		DWORD dwMilliseconds) override;
	DWORD WaitForSingleObject(
		HANDLE hHandle,
		DWORD  dwMilliseconds) override;

	HANDLE CreateEventW(
		LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset,
		BOOL bInitialState,
		LPCWSTR lpName) override;
	BOOL SetEvent(
		HANDLE hEvent) override;
	BOOL ResetEvent(
		HANDLE hEvent) override;

	BOOL CloseHandle(
		HANDLE hObject) override;

	BOOL GetOverlappedResult(
		HANDLE hFile,
		LPOVERLAPPED lpOverlapped,
		LPDWORD lpNumberOfBytesTransferred,
		BOOL bWait) override;

	HRESULT CoCreateGuid(
		GUID* pguid) override;

	HRESULT SHGetKnownFolderPath(
		REFKNOWNFOLDERID rfid,
		DWORD dwFlags,
		HANDLE hToken,
		PWSTR* ppszPath) override;
	void CoTaskMemFree(
		LPVOID pv) override;

	HANDLE CreateNamedPipeW(
		LPCWSTR lpName,
		DWORD dwOpenMode,
		DWORD dwPipeMode,
		DWORD nMaxInstances,
		DWORD nOutBufferSize,
		DWORD nInBufferSize,
		DWORD nDefaultTimeOut,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes) override;
	BOOL ConnectNamedPipe(
		HANDLE hNamedPipe,
		LPOVERLAPPED lpOverlapped) override;

	HANDLE CreateFileW(
		LPCWSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile) override;
	BOOL ReadFile(
		HANDLE hFile,
		LPVOID lpBuffer,
		DWORD nNumberOfBytesToRead,
		LPDWORD lpNumberOfBytesRead,
		LPOVERLAPPED lpOverlapped) override;
	BOOL WriteFile(
		HANDLE hFile,
		LPCVOID lpBuffer,
		DWORD nNumberOfBytesToWrite,
		LPDWORD lpNumberOfBytesWritten,
		LPOVERLAPPED lpOverlapped) override;

	HANDLE GetCurrentProcess() override;
	DWORD GetProcessId(
		HANDLE Process) override;

	BOOL CreateProcessW(
		LPCWSTR lpApplicationName,
		LPWSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation) override;
	BOOL TerminateProcess(
		HANDLE hProcess,
		UINT uExitCode) override;
	BOOL GetExitCodeProcess(
		HANDLE hProcess,
		LPDWORD lpExitCode) override;

	HANDLE GetStdHandle(
		DWORD nStdHandle) override;
	BOOL SetConsoleTextAttribute(
		HANDLE hConsoleOutput,
		WORD wAttributes) override;

	VOID OutputDebugStringW(
		LPCWSTR lpOutputString) override;
};
