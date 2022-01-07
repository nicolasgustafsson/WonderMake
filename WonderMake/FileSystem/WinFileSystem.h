#pragma once

#include "FileSystem/FileSystem.h"

#include "WonderMakeEngine/WinPlatformSystem.h"

#include "Utilities/Container/Container.h"

class WinPlatformSystem;

class WinFileSystem final
	: public FileSystem
	, public SystemSub<
		Policy::Set<
			PAdd<WinPlatformSystem, PWrite>>>
{
public:
	WinFileSystem();

	void SetFolderSuffix(const FolderLocation aLocation, std::filesystem::path aSuffix) override;

	std::optional<std::filesystem::path> GetFolderLocation(const FolderLocation aLocation) override;

private:
	std::optional<std::filesystem::path> GetFolder(REFKNOWNFOLDERID aFolderId);

	std::filesystem::path myBinPath;
	Container<std::filesystem::path, Key<FolderLocation>, Associative> mySuffixes;

};
