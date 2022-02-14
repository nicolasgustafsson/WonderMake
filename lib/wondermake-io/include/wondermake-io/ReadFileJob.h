#pragma once

#include "wondermake-io/FileSystem.h"
#include "wondermake-io/FileTypes.h"

#include "wondermake-base/Job.h"

#include <filesystem>
#include <vector>

enum class ReadFileError
{
	InvalidArguments,
	FileNotFound,
	NotAFile,
	FailedToOpen
};

class ReadFileJob
	: public Job<
		Policy::Set<
			PAdd<FileSystem, PWrite>>,
		JobOutput<std::vector<u8>>,
		JobOutputError<ReadFileError>>
{
public:
	void Run(const FolderLocation aLocation, const std::filesystem::path& aFilePath);

};
