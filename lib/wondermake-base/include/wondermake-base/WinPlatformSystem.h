#pragma once

#include "wondermake-utility/WinPlatform.h"

#include "wondermake-base/System.h"

class WinPlatformSystem
	: public SystemAbstracted
{
public:
	virtual DWORD GetLastError() = 0;

	virtual DWORD WaitForMultipleObjects(
		DWORD nCount,
		const HANDLE* lpHandles,
		BOOL bWaitAll,
		DWORD dwMilliseconds) = 0;
	virtual DWORD WaitForSingleObject(
		HANDLE hHandle,
		DWORD  dwMilliseconds) = 0;

	virtual HANDLE CreateEventW(
		LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset,
		BOOL bInitialState,
		LPCWSTR lpName) = 0;
	virtual BOOL SetEvent(
		HANDLE hEvent) = 0;
	virtual BOOL ResetEvent(
		HANDLE hEvent) = 0;

	virtual BOOL CloseHandle(
		HANDLE hObject) = 0;

	virtual BOOL GetOverlappedResult(
		HANDLE hFile,
		LPOVERLAPPED lpOverlapped,
		LPDWORD lpNumberOfBytesTransferred,
		BOOL bWait) = 0;

	virtual HRESULT CoCreateGuid(
		GUID* pguid) = 0;

	virtual HRESULT SHGetKnownFolderPath(
		REFKNOWNFOLDERID rfid,
		DWORD dwFlags,
		HANDLE hToken,
		PWSTR* ppszPath) = 0;
	virtual void CoTaskMemFree(
		LPVOID pv) = 0;

	virtual HANDLE CreateNamedPipeW(
		LPCWSTR lpName,
		DWORD dwOpenMode,
		DWORD dwPipeMode,
		DWORD nMaxInstances,
		DWORD nOutBufferSize,
		DWORD nInBufferSize,
		DWORD nDefaultTimeOut,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes) = 0;
	virtual BOOL ConnectNamedPipe(
		HANDLE hNamedPipe,
		LPOVERLAPPED lpOverlapped) = 0;

	virtual HANDLE CreateFileW(
		LPCWSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile) = 0;
	virtual BOOL ReadFile(
		HANDLE hFile,
		LPVOID lpBuffer,
		DWORD nNumberOfBytesToRead,
		LPDWORD lpNumberOfBytesRead,
		LPOVERLAPPED lpOverlapped) = 0;
	virtual BOOL WriteFile(
		HANDLE hFile,
		LPCVOID lpBuffer,
		DWORD nNumberOfBytesToWrite,
		LPDWORD lpNumberOfBytesWritten,
		LPOVERLAPPED lpOverlapped) = 0;

	virtual BOOL CreateProcessW(
		LPCWSTR lpApplicationName,
		LPWSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation) = 0;
	virtual BOOL TerminateProcess(
		HANDLE hProcess,
		UINT uExitCode) = 0;
	virtual BOOL GetExitCodeProcess(
		HANDLE hProcess,
		LPDWORD lpExitCode) = 0;

	virtual HANDLE GetStdHandle(
		DWORD nStdHandle) = 0;
	virtual BOOL SetConsoleTextAttribute(
		HANDLE hConsoleOutput,
		WORD wAttributes) = 0;

	virtual VOID OutputDebugStringW(
			LPCWSTR lpOutputString) = 0;
};
