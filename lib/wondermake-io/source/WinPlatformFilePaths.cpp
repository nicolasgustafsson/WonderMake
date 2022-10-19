#include "wondermake-io/PlatformFilePaths.h"

#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/WinPlatform.h"

std::optional<std::filesystem::path> GetFolder(const SystemContainer& aContainer, REFKNOWNFOLDERID aFolderId)
{
	struct PwStrWrapper
	{
		inline ~PwStrWrapper()
		{
			if (Path)
				WinPlatformSys.CoTaskMemFree(Path);
		}

		WinPlatformSystem& WinPlatformSys;
		PWSTR Path = NULL;
	};

	auto& winPlatformSys = aContainer.Get<WinPlatformSystem>();

	PwStrWrapper pathWrapper{ winPlatformSys };

	const auto result = winPlatformSys.SHGetKnownFolderPath(aFolderId, 0, NULL, &pathWrapper.Path);

	if (FAILED(result))
		return std::nullopt;

	return std::filesystem::path(pathWrapper.Path);
}

std::optional<std::filesystem::path> PlatformFilePaths::GetFolderLocation(const SystemContainer& aContainer, const FolderLocation aLocation)
{
	switch (aLocation)
	{
	case FolderLocation::Bin:		return std::filesystem::current_path();
	case FolderLocation::Data:		return GetFolder(aContainer, FOLDERID_ProgramData);
	case FolderLocation::User:		return GetFolder(aContainer, FOLDERID_Documents);
	case FolderLocation::UserData:	return GetFolder(aContainer, FOLDERID_UserProgramFiles);
	}

	return std::nullopt;
}
