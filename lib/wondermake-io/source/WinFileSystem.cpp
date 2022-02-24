#include "WinFileSystem.h"

#include "wondermake-io/ConfigurationIo.h"

#include "wondermake-base/ConfigurationSystem.h"
#include "wondermake-base/SystemGlobal.h"

REGISTER_SYSTEM_MASKED(WinFileSystem, FileSystem);

void WinFileSystem::Initialize()
{
	myBinPath = std::filesystem::current_path();
}

void WinFileSystem::SetFolderSuffix(const FolderLocation aLocation, std::filesystem::path aSuffix)
{
	mySuffixes.emplace(aLocation, std::move(aSuffix));
}

std::optional<std::filesystem::path> WinFileSystem::GetFolderLocation(const FolderLocation aLocation)
{
	std::optional<std::filesystem::path> retVal;

	const auto getPath = [this, aLocation](const auto& aConfigName, const auto& aFolderId) -> std::optional<std::filesystem::path>
	{
		auto config = Get<ConfigurationSystem>().Get<std::string>(static_cast<std::string>(aConfigName), "");

		if (!config.empty())
			return config;

		auto retVal = GetFolder(aFolderId);

		if (!retVal)
			return std::nullopt;

		return *retVal / mySuffixes[aLocation];
	};

	switch (aLocation)
	{
	case FolderLocation::Bin:		return myBinPath / mySuffixes[aLocation];
	case FolderLocation::Data:		return getPath(ConfigurationIo::ConfigDirectoryData,		FOLDERID_ProgramData);
	case FolderLocation::User:		return getPath(ConfigurationIo::ConfigDirectoryUser,		FOLDERID_Documents);
	case FolderLocation::UserData:	return getPath(ConfigurationIo::ConfigDirectoryUserData,	FOLDERID_UserProgramFiles);
	}

	return std::nullopt;
}

std::optional<std::filesystem::path> WinFileSystem::GetFolder(REFKNOWNFOLDERID aFolderId)
{
	struct PwStrWrapper
	{
		inline ~PwStrWrapper()
		{
			if (Path)
				WinPlatform.CoTaskMemFree(Path);
		}
		
		WinPlatformSystem& WinPlatform;
		PWSTR Path = NULL;
	};

	PwStrWrapper pathWrapper{ Get<WinPlatformSystem>() };

	const auto result = Get<WinPlatformSystem>().SHGetKnownFolderPath(aFolderId, 0, NULL, &pathWrapper.Path);

	if (FAILED(result))
		return std::nullopt;

	return std::filesystem::path(pathWrapper.Path);
}
