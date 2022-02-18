#pragma once

#include "wondermake-io/FileTypes.h"

#include "wondermake-base/jobs/JobBase.h"

#include "wondermake-utility/Result.h"

#include <filesystem>
#include <vector>

class FileSystem;

enum class ReadFileError
{
	InvalidArguments,
	FileNotFound,
	NotAFile,
	FailedToOpen
};

class ReadFileJob
	: public jobs_refactor::Job<
		jobs_refactor::JobInputSet<
			jobs_refactor::JobInput<FolderLocation, std::filesystem::path>>,
		Result<ReadFileError, std::vector<u8>>,
		Policy::Set<
			PAdd<FileSystem, PWrite>>>
{
public:
	void Run(Promise<Result<ReadFileError, std::vector<u8>>> aPromise, FolderLocation aLocation, std::filesystem::path aFilePath) override;

};
