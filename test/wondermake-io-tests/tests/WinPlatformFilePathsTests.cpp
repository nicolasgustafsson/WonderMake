#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "wondermake-io/PlatformFilePaths.h"

#include "wondermake-base/SystemContainer.h"

constexpr auto CreateSetPath = [](PWSTR aPath, HRESULT aResult)
{
	return[aPath, aResult](auto&&, auto&&, auto&&, PWSTR* ppszPath)
	{
		*ppszPath = aPath;

		return aResult;
	};
};

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_bin_folder)
{
	SystemContainer container;
	auto winPlatformSys = std::make_shared<NiceMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	const auto path = PlatformFilePaths::GetFolderLocation(container, FolderLocation::Bin);

	EXPECT_EQ(path, std::filesystem::current_path());
}

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_data_folder)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::Data;

	std::wstring				dummyPath		= L"TEST";
	constexpr REFKNOWNFOLDERID	dummyFolderId	= FOLDERID_ProgramData;
	constexpr DWORD				dummyFlags		= 0;
	constexpr HANDLE			dummyToken		= NULL;

	std::wstring				expectedPath	= L"TEST";

	SystemContainer container;
	auto winPlatformSys = std::make_shared<NiceMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	EXPECT_CALL(*winPlatformSys, SHGetKnownFolderPath(dummyFolderId, dummyFlags, dummyToken, _))
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(*winPlatformSys, CoTaskMemFree(dummyPath.data()));

	const auto path = PlatformFilePaths::GetFolderLocation(container, folderLocation);

	EXPECT_EQ(path, expectedPath);
}

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_user_folder)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::User;

	std::wstring				dummyPath		= L"TEST";
	constexpr REFKNOWNFOLDERID	dummyFolderId	= FOLDERID_Documents;
	constexpr DWORD				dummyFlags		= 0;
	constexpr HANDLE			dummyToken		= NULL;

	std::wstring				expectedPath	= L"TEST";

	SystemContainer container;
	auto winPlatformSys = std::make_shared<NiceMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	EXPECT_CALL(*winPlatformSys, SHGetKnownFolderPath(dummyFolderId, dummyFlags, dummyToken, _))
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(*winPlatformSys, CoTaskMemFree(dummyPath.data()));

	const auto path = PlatformFilePaths::GetFolderLocation(container, folderLocation);

	EXPECT_EQ(path, expectedPath);
}

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_user_data_folder)
{
	constexpr FolderLocation	folderLocation	= FolderLocation::UserData;

	std::wstring				dummyPath		= L"TEST";
	constexpr REFKNOWNFOLDERID	dummyFolderId	= FOLDERID_UserProgramFiles;
	constexpr DWORD				dummyFlags		= 0;
	constexpr HANDLE			dummyToken		= NULL;

	std::wstring				expectedPath	= L"TEST";

	SystemContainer container;
	auto winPlatformSys = std::make_shared<NiceMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	EXPECT_CALL(*winPlatformSys, SHGetKnownFolderPath(dummyFolderId, dummyFlags, dummyToken, _))
		.WillOnce(CreateSetPath(dummyPath.data(), S_OK));
	EXPECT_CALL(*winPlatformSys, CoTaskMemFree(dummyPath.data()));

	const auto path = PlatformFilePaths::GetFolderLocation(container, folderLocation);

	EXPECT_EQ(path, expectedPath);
}

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_nullopt_on_failure_when_getting_data_folder)
{
	SystemContainer container;
	auto winPlatformSys = std::make_shared<StrictMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	EXPECT_CALL(*winPlatformSys, SHGetKnownFolderPath)
		.WillOnce(Return(E_FAIL));

	const auto path = PlatformFilePaths::GetFolderLocation(container, FolderLocation::Data);

	EXPECT_FALSE(path);
}

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_nullopt_on_failure_when_getting_user_folder)
{
	SystemContainer container;
	auto winPlatformSys = std::make_shared<StrictMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	EXPECT_CALL(*winPlatformSys, SHGetKnownFolderPath)
		.WillOnce(Return(E_FAIL));

	const auto path = PlatformFilePaths::GetFolderLocation(container, FolderLocation::User);

	EXPECT_FALSE(path);
}

TEST(WinPlatformFilePathsTests, getfolderlocation_returns_nullopt_on_failure_when_getting_user_data_folder)
{
	SystemContainer container;
	auto winPlatformSys = std::make_shared<StrictMock<WinPlatformSystemMock>>();

	container.Add<WinPlatformSystem>(winPlatformSys);

	EXPECT_CALL(*winPlatformSys, SHGetKnownFolderPath)
		.WillOnce(Return(E_FAIL));

	const auto path = PlatformFilePaths::GetFolderLocation(container, FolderLocation::UserData);

	EXPECT_FALSE(path);
}
