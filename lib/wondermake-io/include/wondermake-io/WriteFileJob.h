#pragma once

#include "wondermake-io/FileTypes.h"

#include "wondermake-base/JobBase.h"

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
			JobInput<FolderLocation, std::filesystem::path, std::vector<u8>>,
			JobInput<FolderLocation, std::filesystem::path, std::string>>,
		Result<void, EWriteFileError>>
{};
