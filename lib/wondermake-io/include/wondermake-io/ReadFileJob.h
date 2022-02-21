#pragma once

#include "wondermake-io/FileTypes.h"

#include "wondermake-base/JobBase.h"

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
	: public JobAbstracted<
		JobInputSet<
			JobInput<FolderLocation, std::filesystem::path>>,
		Result<ReadFileError, std::vector<u8>>>
{};
