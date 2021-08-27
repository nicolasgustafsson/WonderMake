#define _ITERATOR_DEBUG_LEVEL 0

#include <catch2/catch.hpp>

#include "FileSystem/WinFileSystem.h"

class PlatformWindowsSystemMock
	: public WinPlatformSystem
{
public:
	HRESULT SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath) override
	{
		++myCallCountGet;

		CHECK(&rfid == myRfid);
		CHECK(dwFlags == myDwFlags);
		CHECK(hToken == myHToken);
		REQUIRE(ppszPath != NULL);

		if (myPathSet)
			*ppszPath = myPath;

		return myReturnValue;
	}

	void CoTaskMemFree(LPVOID pv) override
	{
		++myCallCountFree;

		CHECK(pv == myPath);
	}

	void SetExpectedArgs(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken)
	{
		myRfid = &rfid;
		myDwFlags = dwFlags;
		myHToken = hToken;
	}
	void SetReturnValue(HRESULT aResult)
	{
		myReturnValue = aResult;
	}
	void SetPath(std::wstring_view aPath)
	{
		myPathSet = true;
		memcpy(myPath, aPath.data(), aPath.size() * sizeof(WCHAR));
	}

	u32 CallCountGet() const
	{
		return myCallCountGet;
	}
	u32 CallCountFree() const
	{
		return myCallCountFree;
	}

private:
	u32 myCallCountGet = 0;
	u32 myCallCountFree = 0;

	const KNOWNFOLDERID* myRfid;
	DWORD myDwFlags;
	HANDLE myHToken;
	HRESULT myReturnValue;

	bool myPathSet = false;
	WCHAR myPath[MAX_PATH] = {};
};

TEST_CASE("GetFolderLocation returns bin folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(fileSystem.GetFolderLocation(FolderLocation::Bin) == (std::filesystem::current_path() / ""));

	CHECK(mock.CallCountGet() == 0);
	CHECK(mock.CallCountFree() == 0);
}

TEST_CASE("GetFolderLocation returns data folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_AppDataProgramData, 0, NULL);
	mock.SetReturnValue(S_OK);
	mock.SetPath(L"TEST");

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(fileSystem.GetFolderLocation(FolderLocation::Data) == L"TEST//");

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 1);
}

TEST_CASE("GetFolderLocation returns user folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_AppDataDocuments, 0, NULL);
	mock.SetReturnValue(S_OK);
	mock.SetPath(L"TEST");

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(fileSystem.GetFolderLocation(FolderLocation::User) == L"TEST//");

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 1);
}

TEST_CASE("GetFolderLocation returns user data folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_UserProgramFiles, 0, NULL);
	mock.SetReturnValue(S_OK);
	mock.SetPath(L"TEST");

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(fileSystem.GetFolderLocation(FolderLocation::UserData) == L"TEST//");

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 1);
}

TEST_CASE("GetFolderLocation returns bin folder with suffix.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	fileSystem.SetFolderSuffix(FolderLocation::Bin, "suffix");

	CHECK(fileSystem.GetFolderLocation(FolderLocation::Bin) == (std::filesystem::current_path() / "suffix"));

	CHECK(mock.CallCountGet() == 0);
	CHECK(mock.CallCountFree() == 0);
}

TEST_CASE("GetFolderLocation returns data folder with suffix.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_AppDataProgramData, 0, NULL);
	mock.SetReturnValue(S_OK);
	mock.SetPath(L"TEST");

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	fileSystem.SetFolderSuffix(FolderLocation::Data, "suffix");

	CHECK(fileSystem.GetFolderLocation(FolderLocation::Data) == L"TEST//suffix");

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 1);
}

TEST_CASE("GetFolderLocation returns user folder with suffix.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_AppDataDocuments, 0, NULL);
	mock.SetReturnValue(S_OK);
	mock.SetPath(L"TEST");

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	fileSystem.SetFolderSuffix(FolderLocation::User, "suffix");

	CHECK(fileSystem.GetFolderLocation(FolderLocation::User) == L"TEST//suffix");

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 1);
}

TEST_CASE("GetFolderLocation returns user data folder with suffix.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_UserProgramFiles, 0, NULL);
	mock.SetReturnValue(S_OK);
	mock.SetPath(L"TEST");

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	fileSystem.SetFolderSuffix(FolderLocation::UserData, "suffix");

	CHECK(fileSystem.GetFolderLocation(FolderLocation::UserData) == L"TEST//suffix");

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 1);
}

TEST_CASE("GetFolderLocation returns nullopt on failure when getting data folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_AppDataProgramData, 0, NULL);
	mock.SetReturnValue(E_FAIL);

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(!fileSystem.GetFolderLocation(FolderLocation::Data));

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 0);
}

TEST_CASE("GetFolderLocation returns nullopt on failure when getting user folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_AppDataDocuments, 0, NULL);
	mock.SetReturnValue(E_FAIL);

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(!fileSystem.GetFolderLocation(FolderLocation::User));

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 0);
}

TEST_CASE("GetFolderLocation returns nullopt on failure when getting user data folder.", "[WinFileSystem]")
{
	PlatformWindowsSystemMock mock;

	mock.SetExpectedArgs(FOLDERID_UserProgramFiles, 0, NULL);
	mock.SetReturnValue(E_FAIL);

	WinFileSystem::InjectDependencies(std::tie(mock));

	WinFileSystem fileSystem;

	CHECK(!fileSystem.GetFolderLocation(FolderLocation::UserData));

	CHECK(mock.CallCountGet() == 1);
	CHECK(mock.CallCountFree() == 0);
}
