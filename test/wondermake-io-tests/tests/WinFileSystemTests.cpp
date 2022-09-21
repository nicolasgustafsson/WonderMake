#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/ConfigurationSystem.h"

#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WinFileSystem.h"

constexpr auto CreateSetPath = [](PWSTR aPath, HRESULT aResult)
{
	return[aPath, aResult](auto&&, auto&&, auto&&, PWSTR* ppszPath)
	{
		*ppszPath = aPath;

		return aResult;
	};
};

TEST(WinFileSystemTests, getfolderlocation_returns_bin_folder)
{
	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_EQ(fileSystem.GetFolderLocation(FolderLocation::Bin), (std::filesystem::current_path() / ""));
}

TEST(WinFileSystemTests, getfolderlocation_returns_data_folder)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::Data;

	std::wstring				dummyPath		= L"TEST";
	constexpr REFKNOWNFOLDERID	dummyFolderId	= FOLDERID_ProgramData;
	constexpr DWORD				dummyFlags		= 0;
	constexpr HANDLE			dummyToken		= NULL;

	std::wstring				expectedPath	= L"TEST//";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_CALL(mock, SHGetKnownFolderPath(dummyFolderId, dummyFlags, dummyToken, _))
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(mock, CoTaskMemFree(dummyPath.data()));

	EXPECT_EQ(fileSystem.GetFolderLocation(folderLocation), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_user_folder)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::User;

	std::wstring				dummyPath		= L"TEST";
	constexpr REFKNOWNFOLDERID	dummyFolderId	= FOLDERID_Documents;
	constexpr DWORD				dummyFlags		= 0;
	constexpr HANDLE			dummyToken		= NULL;

	std::wstring				expectedPath	= L"TEST//";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_CALL(mock, SHGetKnownFolderPath(dummyFolderId, dummyFlags, dummyToken, _))
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(mock, CoTaskMemFree(dummyPath.data()));

	EXPECT_EQ(fileSystem.GetFolderLocation(folderLocation), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_user_data_folder)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::UserData;

	std::wstring				dummyPath		= L"TEST";
	constexpr REFKNOWNFOLDERID	dummyFolderId	= FOLDERID_UserProgramFiles;
	constexpr DWORD				dummyFlags		= 0;
	constexpr HANDLE			dummyToken		= NULL;

	std::wstring				expectedPath	= L"TEST//";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_CALL(mock, SHGetKnownFolderPath(dummyFolderId, dummyFlags, dummyToken, _))
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(mock, CoTaskMemFree(dummyPath.data()));

	EXPECT_EQ(fileSystem.GetFolderLocation(folderLocation), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_bin_folder_with_suffix)
{
	auto expectedPath = std::filesystem::current_path() / "suffix";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	fileSystem.SetFolderSuffix(FolderLocation::Bin, "suffix");

	EXPECT_EQ(fileSystem.GetFolderLocation(FolderLocation::Bin), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_data_folder_with_suffix)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::Data;
	std::wstring				dummyPath		= L"TEST";
	std::wstring				expectedPath	= L"TEST//suffix";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	fileSystem.SetFolderSuffix(FolderLocation::Data, "suffix");

	EXPECT_CALL(mock, SHGetKnownFolderPath)
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(mock, CoTaskMemFree(dummyPath.data()));

	EXPECT_EQ(fileSystem.GetFolderLocation(folderLocation), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_user_folder_with_suffix)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::User;
	std::wstring				dummyPath		= L"TEST";
	std::wstring				expectedPath	= L"TEST//suffix";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	fileSystem.SetFolderSuffix(FolderLocation::User, "suffix");

	EXPECT_CALL(mock, SHGetKnownFolderPath)
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(mock, CoTaskMemFree(dummyPath.data()));

	EXPECT_EQ(fileSystem.GetFolderLocation(folderLocation), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_user_data_folder_with_suffix)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::UserData;
	std::wstring				dummyPath		= L"TEST";
	std::wstring				expectedPath	= L"TEST//suffix";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	fileSystem.SetFolderSuffix(FolderLocation::UserData, "suffix");

	EXPECT_CALL(mock, SHGetKnownFolderPath)
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(mock, CoTaskMemFree(dummyPath.data()));

	EXPECT_EQ(fileSystem.GetFolderLocation(folderLocation), expectedPath);
}

TEST(WinFileSystemTests, getfolderlocation_returns_nullopt_on_failure_when_getting_data_folder)
{
	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	StrictMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_CALL(mock, SHGetKnownFolderPath)
		.WillOnce(Return(E_FAIL));

	EXPECT_FALSE(fileSystem.GetFolderLocation(FolderLocation::Data));
}

TEST(WinFileSystemTests, getfolderlocation_returns_nullopt_on_failure_when_getting_user_folder)
{
	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_CALL(mock, SHGetKnownFolderPath)
		.WillOnce(Return(E_FAIL));

	EXPECT_FALSE(fileSystem.GetFolderLocation(FolderLocation::User));
}

TEST(WinFileSystemTests, getfolderlocation_returns_nullopt_on_failure_when_getting_user_data_folder)
{
	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem config;

	NiceMock<WinPlatformSystemMock> mock;

	WinFileSystem::InjectDependencies(std::tie(config, mock));

	WinFileSystem fileSystem;

	fileSystem.Initialize();

	EXPECT_CALL(mock, SHGetKnownFolderPath)
		.WillOnce(Return(E_FAIL));

	EXPECT_FALSE(fileSystem.GetFolderLocation(FolderLocation::UserData));
}
