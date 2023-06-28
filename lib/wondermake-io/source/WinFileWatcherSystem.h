#pragma once

#include "wondermake-utility/WinPlatform.h"

#include "wondermake-io/FileWatcherSystem.h"

#include "wondermake-base/WinEventHandle.h"

#include <array>
#include <string>
#include <unordered_map>

class ConfigurationSystem;
class WinEventSystem;
class WinPlatformSystem;

class WinFileWatcherSystem
	: public FileWatcherSystem
	, public SystemSub<
		Policy::Set<
			PAdd<ConfigurationSystem, PWrite>,
			PAdd<WinEventSystem, PWrite>,
			PAdd<WinPlatformSystem, PWrite>>,
		STrait::Set<
			STFoundational,
			STSingleton,
			STWonderMake>>
{
public:
	WinFileWatcherSystem();

	void Initialize() override;

protected:
	void RegisterDirWatch(std::filesystem::path aDirPath) override;
	void UnregisterDirWatch(std::filesystem::path aDirPath) override;

private:
	struct SDirectoryData
	{
		std::filesystem::path	DirPath;
		WinEventHandle			HandleDir;
		WinEventHandle			HandleOverlapped;
		OVERLAPPED				Overlapped = {};
		Future<void>			Future;
		std::array<char, 1024>	Buffer = {};
	};

	void SetCallback(SDirectoryData& aData);
	void ProcessCallback(SDirectoryData& aData);

	std::unordered_map<std::string, std::unique_ptr<SDirectoryData>> myDirectoriesData;

};
