#include "pch.h"
#include "FileSystem/WinFileSystem.h"

REGISTER_SYSTEM_MASKED(WinFileSystem, FileSystem);

WinFileSystem::WinFileSystem()
	: myBinPath(std::filesystem::current_path())
{}

void WinFileSystem::SetFolderSuffix(const FolderLocation aLocation, std::filesystem::path aSuffix)
{
	mySuffixes.Add(aLocation, std::move(aSuffix));
}

std::optional<std::filesystem::path> WinFileSystem::GetFolderLocation(const FolderLocation aLocation)
{
	std::optional<std::filesystem::path> retVal;

	switch (aLocation)
	{
	case FolderLocation::Bin:		retVal = myBinPath; break;
	case FolderLocation::Data:		retVal = GetFolder(FOLDERID_AppDataProgramData); break;
	case FolderLocation::User:		retVal = GetFolder(FOLDERID_AppDataDocuments); break;
	case FolderLocation::UserData:	retVal = GetFolder(FOLDERID_UserProgramFiles); break;
	}

	if (retVal)
		*retVal /= mySuffixes[aLocation];

	return retVal;
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

	if (FAILED(Get<WinPlatformSystem>().SHGetKnownFolderPath(aFolderId, 0, NULL, &pathWrapper.Path)))
		return std::nullopt;

	return std::filesystem::path(pathWrapper.Path);
}
