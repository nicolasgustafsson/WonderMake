#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/FileWatcherSystem.h"

class FileWatcherSystemMock
	: public FileWatcherSystem
{
public:
	FileWatcherSystemMock(const FilePathData& aFilePathData)
		: FileWatcherSystem(InlineExecutor(), aFilePathData)
	{}

	inline void Initialize()
	{
		FileWatcherSystem::Initialize();
	}

	MOCK_METHOD(void, RegisterDirWatch, (std::filesystem::path), (override));
	MOCK_METHOD(void, UnregisterDirWatch, (std::filesystem::path), (override));

	inline void ExternalTrigger(const std::filesystem::path& aFilePath)
	{
		TriggerOnFileChange(aFilePath);
	}
};
