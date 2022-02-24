#pragma once

#include "wondermake-io/FileSystem.h"

#include "wondermake-base/WinPlatformSystem.h"

#include <unordered_map>

class WinPlatformSystem;

class ConfigurationSystem;

class WinFileSystem final
	: public FileSystem
	, public SystemSub<
		Policy::Set<
			PAdd<ConfigurationSystem, PRead>,
			PAdd<WinPlatformSystem, PWrite>>,
		STrait::Set<
			STFoundational,
			STSingleton>>
{
public:
	void Initialize() override;

	void SetFolderSuffix(const FolderLocation aLocation, std::filesystem::path aSuffix) override;

	std::optional<std::filesystem::path> GetFolderLocation(const FolderLocation aLocation) override;

private:
	std::optional<std::filesystem::path> GetFolder(REFKNOWNFOLDERID aFolderId);

	std::filesystem::path myBinPath;
	std::unordered_map<FolderLocation, std::filesystem::path> mySuffixes;

};
