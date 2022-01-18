#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "Guid/WinGuidGeneratorSystem.h"
#include "WonderMakeEngine/WinPlatform.h"
#include "WonderMakeEngine/WinPlatformSystem.h"

constexpr Guid GuidData = std::array<u8, 16> { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

TEST_CASE("GenerateNew returns a valid Guid.", "[WinGuidGeneratorSystem]")
{
	class PlatformWindowsSystemMock
		: public WinPlatformSystem
	{
	public:
		DWORD GetLastError()
		{
			return 0;
		}

		DWORD WaitForMultipleObjects(
			DWORD /*nCount*/,
			const HANDLE* /*lpHandles*/,
			BOOL /*bWaitAll*/,
			DWORD /*dwMilliseconds*/)
		{
			return 0;
		}

		DWORD WaitForSingleObject(
			HANDLE /*hHandle*/,
			DWORD  /*dwMilliseconds*/)
		{
			return 0;
		}

		HANDLE CreateEventW(
			LPSECURITY_ATTRIBUTES /*lpEventAttributes*/,
			BOOL /*bManualReset*/,
			BOOL /*bInitialState*/,
			LPCWSTR /*lpName*/) override
		{
			return NULL;
		}

		BOOL SetEvent(
			HANDLE /*hEvent*/)
		{
			return FALSE;
		}

		BOOL ResetEvent(
			HANDLE /*hEvent*/)
		{
			return FALSE;
		}

		BOOL CloseHandle(
			HANDLE /*hObject*/)
		{
			return FALSE;
		}

		BOOL GetOverlappedResult(
			HANDLE /*hFile*/,
			LPOVERLAPPED /*lpOverlapped*/,
			LPDWORD /*lpNumberOfBytesTransferred*/,
			BOOL /*bWait*/)
		{
			return FALSE;
		}

		HRESULT CoCreateGuid(GUID* pguid) override
		{
			memcpy(pguid, GuidData.data(), GuidData.size());

			return S_OK;
		}

		HRESULT SHGetKnownFolderPath(
			REFKNOWNFOLDERID /*rfid*/,
			DWORD /*dwFlags*/,
			HANDLE /*hToken*/,
			PWSTR* /*ppszPath*/ ) override
		{
			return S_OK;
		}

		void CoTaskMemFree(LPVOID /*pv*/) override {}

		HANDLE CreateNamedPipeW(
			LPCWSTR /*lpName*/,
			DWORD /*dwOpenMode*/,
			DWORD /*dwPipeMode*/,
			DWORD /*nMaxInstances*/,
			DWORD /*nOutBufferSize*/,
			DWORD /*nInBufferSize*/,
			DWORD /*nDefaultTimeOut*/,
			LPSECURITY_ATTRIBUTES /*lpSecurityAttributes*/)
		{
			return NULL;
		}

		BOOL ConnectNamedPipe(
			HANDLE /*hNamedPipe*/,
			LPOVERLAPPED /*lpOverlapped*/)
		{
			return FALSE;
		}

		HANDLE CreateFileW(
			LPCWSTR /*lpFileName*/,
			DWORD /*dwDesiredAccess*/,
			DWORD /*dwShareMode*/,
			LPSECURITY_ATTRIBUTES /*lpSecurityAttributes*/,
			DWORD /*dwCreationDisposition*/,
			DWORD /*dwFlagsAndAttributes*/,
			HANDLE /*hTemplateFile*/) override

		{
			return NULL;
		}

		BOOL ReadFile(
			HANDLE /*hFile*/,
			LPVOID /*lpBuffer*/,
			DWORD /*nNumberOfBytesToRead*/,
			LPDWORD /*lpNumberOfBytesRead*/,
			LPOVERLAPPED /*lpOverlapped*/)
		{
			return FALSE;
		}

		BOOL WriteFile(
			HANDLE /*hFile*/,
			LPCVOID /*lpBuffer*/,
			DWORD /*nNumberOfBytesToWrite*/,
			LPDWORD /*lpNumberOfBytesWritten*/,
			LPOVERLAPPED /*lpOverlapped*/)
		{
			return FALSE;
		}

	};

	PlatformWindowsSystemMock mock;

	WinGuidGeneratorSystem::InjectDependencies(std::tie(mock));

	WinGuidGeneratorSystem guidGenerator;

	const auto guid = guidGenerator.GenerateNew();

	REQUIRE(guid);
	REQUIRE(*guid == GuidData);
}

TEST_CASE("GenerateNew returns nullopt on error.", "[WinGuidGeneratorSystem]")
{
	class PlatformWindowsSystemMock
		: public WinPlatformSystem
	{
	public:
		DWORD GetLastError()
		{
			return 0;
		}

		DWORD WaitForMultipleObjects(
			DWORD /*nCount*/,
			const HANDLE* /*lpHandles*/,
			BOOL /*bWaitAll*/,
			DWORD /*dwMilliseconds*/)
		{
			return 0;
		}

		DWORD WaitForSingleObject(
			HANDLE /*hHandle*/,
			DWORD  /*dwMilliseconds*/)
		{
			return 0;
		}

		HANDLE CreateEventW(
			LPSECURITY_ATTRIBUTES /*lpEventAttributes*/,
			BOOL /*bManualReset*/,
			BOOL /*bInitialState*/,
			LPCWSTR /*lpName*/) override
		{
			return NULL;
		}

		BOOL SetEvent(
			HANDLE /*hEvent*/)
		{
			return FALSE;
		}

		BOOL ResetEvent(
			HANDLE /*hEvent*/)
		{
			return FALSE;
		}

		BOOL CloseHandle(
			HANDLE /*hObject*/)
		{
			return FALSE;
		}

		BOOL GetOverlappedResult(
			HANDLE /*hFile*/,
			LPOVERLAPPED /*lpOverlapped*/,
			LPDWORD /*lpNumberOfBytesTransferred*/,
			BOOL /*bWait*/)
		{
			return FALSE;
		}

		HRESULT CoCreateGuid(GUID* /*pguid*/) override
		{
			return S_FALSE;
		}

		HRESULT SHGetKnownFolderPath(
			REFKNOWNFOLDERID /*rfid*/,
			DWORD /*dwFlags*/,
			HANDLE /*hToken*/,
			PWSTR* /*ppszPath*/) override
		{
			return S_OK;
		}

		void CoTaskMemFree(LPVOID /*pv*/) override {}

		HANDLE CreateNamedPipeW(
			LPCWSTR /*lpName*/,
			DWORD /*dwOpenMode*/,
			DWORD /*dwPipeMode*/,
			DWORD /*nMaxInstances*/,
			DWORD /*nOutBufferSize*/,
			DWORD /*nInBufferSize*/,
			DWORD /*nDefaultTimeOut*/,
			LPSECURITY_ATTRIBUTES /*lpSecurityAttributes*/)
		{
			return NULL;
		}

		BOOL ConnectNamedPipe(
			HANDLE /*hNamedPipe*/,
			LPOVERLAPPED /*lpOverlapped*/)
		{
			return FALSE;
		}

		HANDLE CreateFileW(
			LPCWSTR /*lpFileName*/,
			DWORD /*dwDesiredAccess*/,
			DWORD /*dwShareMode*/,
			LPSECURITY_ATTRIBUTES /*lpSecurityAttributes*/,
			DWORD /*dwCreationDisposition*/,
			DWORD /*dwFlagsAndAttributes*/,
			HANDLE /*hTemplateFile*/) override

		{
			return NULL;
		}

		BOOL ReadFile(
			HANDLE /*hFile*/,
			LPVOID /*lpBuffer*/,
			DWORD /*nNumberOfBytesToRead*/,
			LPDWORD /*lpNumberOfBytesRead*/,
			LPOVERLAPPED /*lpOverlapped*/)
		{
			return FALSE;
		}

		BOOL WriteFile(
			HANDLE /*hFile*/,
			LPCVOID /*lpBuffer*/,
			DWORD /*nNumberOfBytesToWrite*/,
			LPDWORD /*lpNumberOfBytesWritten*/,
			LPOVERLAPPED /*lpOverlapped*/)
		{
			return FALSE;
		}

	};

	PlatformWindowsSystemMock mock;

	WinGuidGeneratorSystem::InjectDependencies(std::tie(mock));

	WinGuidGeneratorSystem guidGenerator;

	const auto guid = guidGenerator.GenerateNew();

	REQUIRE(!guid);
}
