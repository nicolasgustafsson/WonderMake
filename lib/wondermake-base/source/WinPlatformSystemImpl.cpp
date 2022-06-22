#include "wondermake-base/SystemGlobal.h"

#include "WinPlatformSystemImpl.h"

REGISTER_SYSTEM_MASKED(WinPlatformSystemImpl, WinPlatformSystem);

DWORD WinPlatformSystemImpl::GetLastError()
{
	return ::GetLastError();
}

DWORD WinPlatformSystemImpl::WaitForMultipleObjects(
	DWORD nCount,
	const HANDLE* lpHandles,
	BOOL bWaitAll,
	DWORD dwMilliseconds)
{
	return ::WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);
}

DWORD WinPlatformSystemImpl::WaitForSingleObject(
	HANDLE hHandle,
	DWORD  dwMilliseconds)
{
	return ::WaitForSingleObject(hHandle, dwMilliseconds);
}

HANDLE WinPlatformSystemImpl::CreateEventW(
	LPSECURITY_ATTRIBUTES lpEventAttributes,
	BOOL bManualReset,
	BOOL bInitialState,
	LPCWSTR lpName)
{
	return ::CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
}

BOOL WinPlatformSystemImpl::SetEvent(
	HANDLE hEvent)
{
	return ::SetEvent(hEvent);
}

BOOL WinPlatformSystemImpl::ResetEvent(
	HANDLE hEvent)
{
	return ::ResetEvent(hEvent);
}

BOOL WinPlatformSystemImpl::CloseHandle(
	HANDLE hObject)
{
	return ::CloseHandle(hObject);
}

BOOL WinPlatformSystemImpl::GetOverlappedResult(
	HANDLE hFile,
	LPOVERLAPPED lpOverlapped,
	LPDWORD lpNumberOfBytesTransferred,
	BOOL bWait)
{
	return ::GetOverlappedResult(hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait);
}

HRESULT WinPlatformSystemImpl::CoCreateGuid(
	GUID* pguid)
{
	return ::CoCreateGuid(pguid);
}

HRESULT WinPlatformSystemImpl::SHGetKnownFolderPath(
	REFKNOWNFOLDERID rfid,
	DWORD dwFlags,
	HANDLE hToken,
	PWSTR* ppszPath)
{
	return ::SHGetKnownFolderPath(rfid, dwFlags, hToken, ppszPath);
}

void WinPlatformSystemImpl::CoTaskMemFree(
	LPVOID pv)
{
	::CoTaskMemFree(pv);
}

HANDLE WinPlatformSystemImpl::CreateNamedPipeW(
	LPCWSTR lpName,
	DWORD dwOpenMode,
	DWORD dwPipeMode,
	DWORD nMaxInstances,
	DWORD nOutBufferSize,
	DWORD nInBufferSize,
	DWORD nDefaultTimeOut,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	return ::CreateNamedPipeW(lpName, dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize, nInBufferSize, nDefaultTimeOut, lpSecurityAttributes);
}

BOOL WinPlatformSystemImpl::ConnectNamedPipe(
	HANDLE hNamedPipe,
	LPOVERLAPPED lpOverlapped)
{
	return ::ConnectNamedPipe(hNamedPipe, lpOverlapped);
}

HANDLE WinPlatformSystemImpl::CreateFileW(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	return ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL WinPlatformSystemImpl::ReadFile(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	return ::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL WinPlatformSystemImpl::WriteFile(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	return ::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

HANDLE WinPlatformSystemImpl::GetCurrentProcess()
{
	return ::GetCurrentProcess();
}

DWORD WinPlatformSystemImpl::GetProcessId(
	HANDLE Process)
{
	return ::GetProcessId(Process);
}

BOOL WinPlatformSystemImpl::CreateProcessW(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	return ::CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

BOOL WinPlatformSystemImpl::TerminateProcess(
	HANDLE hProcess,
	UINT uExitCode)
{
	return ::TerminateProcess(hProcess, uExitCode);
}

BOOL WinPlatformSystemImpl::GetExitCodeProcess(
	HANDLE hProcess,
	LPDWORD lpExitCode)
{
	return ::GetExitCodeProcess(hProcess, lpExitCode);
}

HANDLE WinPlatformSystemImpl::GetStdHandle(
	DWORD nStdHandle)
{
	return ::GetStdHandle(nStdHandle);
}

BOOL WinPlatformSystemImpl::SetConsoleTextAttribute(
	HANDLE hConsoleOutput,
	WORD wAttributes)
{
	return ::SetConsoleTextAttribute(hConsoleOutput, wAttributes);
}

VOID WinPlatformSystemImpl::OutputDebugStringW(
	LPCWSTR lpOutputString)
{
	return ::OutputDebugStringW(lpOutputString);
}
