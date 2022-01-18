#pragma once

#include "GTestInclude.h"

#include "WonderMakeEngine/WinPlatformSystem.h"

class WinPlatformSystemMock
	: public WinPlatformSystem
{
public:
	MOCK_METHOD(DWORD, GetLastError, (), (override));

	MOCK_METHOD(DWORD, WaitForMultipleObjects, (
		DWORD nCount,
		const HANDLE* lpHandles,
		BOOL bWaitAll,
		DWORD dwMilliseconds), (override));
	MOCK_METHOD(DWORD, WaitForSingleObject, (
		HANDLE hHandle,
		DWORD  dwMilliseconds), (override));

	MOCK_METHOD(HANDLE, CreateEventW, (
		LPSECURITY_ATTRIBUTES lpEventAttributes,
		BOOL bManualReset,
		BOOL bInitialState,
		LPCWSTR lpName), (override));
	MOCK_METHOD(BOOL, SetEvent, (
		HANDLE hEvent), (override));
	MOCK_METHOD(BOOL, ResetEvent, (
		HANDLE hEvent), (override));

	MOCK_METHOD(BOOL, CloseHandle, (
		HANDLE hObject), (override));

	MOCK_METHOD(BOOL, GetOverlappedResult, (
		HANDLE hFile,
		LPOVERLAPPED lpOverlapped,
		LPDWORD lpNumberOfBytesTransferred,
		BOOL bWait), (override));

	MOCK_METHOD(HRESULT, CoCreateGuid, (
		GUID* pguid), (override));

	MOCK_METHOD(HRESULT, SHGetKnownFolderPath, (
		REFKNOWNFOLDERID rfid,
		DWORD dwFlags,
		HANDLE hToken,
		PWSTR* ppszPath), (override));
	MOCK_METHOD(void, CoTaskMemFree, (
		LPVOID pv), (override));

	MOCK_METHOD(HANDLE, CreateNamedPipeW, (
		LPCWSTR lpName,
		DWORD dwOpenMode,
		DWORD dwPipeMode,
		DWORD nMaxInstances,
		DWORD nOutBufferSize,
		DWORD nInBufferSize,
		DWORD nDefaultTimeOut,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes), (override));
	MOCK_METHOD(BOOL, ConnectNamedPipe, (
		HANDLE hNamedPipe,
		LPOVERLAPPED lpOverlapped), (override));

	MOCK_METHOD(HANDLE, CreateFileW, (
		LPCWSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile), (override));
	MOCK_METHOD(BOOL, ReadFile, (
		HANDLE hFile,
		LPVOID lpBuffer,
		DWORD nNumberOfBytesToRead,
		LPDWORD lpNumberOfBytesRead,
		LPOVERLAPPED lpOverlapped), (override));
	MOCK_METHOD(BOOL, WriteFile, (
		HANDLE hFile,
		LPCVOID lpBuffer,
		DWORD nNumberOfBytesToWrite,
		LPDWORD lpNumberOfBytesWritten,
		LPOVERLAPPED lpOverlapped), (override));

};