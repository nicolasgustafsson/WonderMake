#include "wondermake-io/FileWatcherSystem.h"

void FileWatcherSystem::Initialize()
{
	auto registerPaths = [this](const std::vector<std::filesystem::path>& aPaths)
	{
		for (const auto& path : aPaths)
		{
			auto str = path.string();

			if (myRegisteredDirs.contains(str))
				continue;

			RegisterDirWatch(path);

			myRegisteredDirs.emplace(std::move(str));
		}
	};

	registerPaths(myFilePathData.GetAllPathsBin());
	registerPaths(myFilePathData.GetAllPathsData());
	registerPaths(myFilePathData.GetAllPathsUser());
	registerPaths(myFilePathData.GetAllPathsUserData());
}

void FileWatcherSystem::UpdateRegisteredDirs(
	const std::filesystem::path& aPathBin,
	const std::filesystem::path& aPathData,
	const std::filesystem::path& aPathUser,
	const std::filesystem::path& aPathUserData)
{
	std::unordered_set<std::string>	dirs;

	auto registerPath = [&dirs, this](const std::filesystem::path& aPath)
	{
		auto str = aPath.string();

		dirs.emplace(str);

		if (myRegisteredDirs.contains(str))
			return;

		RegisterDirWatch(aPath);

		myRegisteredDirs.emplace(std::move(str));
	};

	auto registerPaths = [&dirs, &registerPath](const std::vector<std::filesystem::path>& aPaths)
	{
		for (const auto& path : aPaths)
			registerPath(path);
	};

	registerPath(myFilePathData.GetPathBin());
	registerPaths(FilePathData::ExtractPaths(aPathBin));
	registerPaths(FilePathData::ExtractPaths(aPathData));
	registerPaths(FilePathData::ExtractPaths(aPathUser));
	registerPaths(FilePathData::ExtractPaths(aPathUserData));

	for (const auto& path : myRegisteredDirs)
		if (!dirs.contains(path))
			UnregisterDirWatch(path);

	myRegisteredDirs = std::move(dirs);
}

void FileWatcherSystem::TriggerOnFileChange(const std::filesystem::path& aPath)
{
	auto resolvedPath = FilePath::Resolve(aPath, myFilePathData);
	
	for (auto& triggerData : myOnFileChangeTriggers)
	{
		if (triggerData.Path != resolvedPath)
			continue;

		triggerData.Trigger.Trigger(resolvedPath);
	}
}
