#pragma once

#include "wondermake-base/JobBase.h"

#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/Result.h"

#include <filesystem>
#include <vector>

class FileSystem;

enum class EWriteFileError
{
	InvalidArguments,
	PathIsAlreadyADirectory,
	FailedToCreateDirectory,
	FailedToOpen
};

class WriteFileJob
	: public JobAbstracted<
		JobInputSet<
			JobInput<FilePath, std::vector<u8>>,
			JobInput<FilePath, std::string>>,
		Result<void, EWriteFileError>>
{};
