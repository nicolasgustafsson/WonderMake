#pragma once

#include "wondermake-base/JobBase.h"

#include "wondermake-utility/FilePath.h"
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
			JobInput<FilePath>>,
		Result<std::vector<u8>, ReadFileError>>
{};
