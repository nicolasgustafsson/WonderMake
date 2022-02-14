#pragma once

#include "wondermake-io/FileTypes.h"
#include "wondermake-base/Job.h"
#include "Utilities/Container/Container.h"

#include <filesystem>

class FileSystem;

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
		JobOutput<Container<u8, Iterable, Indexable, ContiguousElements, EqualityComparable>>,
		JobOutputError<ReadFileError>>
{
public:
	void Run(const FolderLocation aLocation, const std::filesystem::path& aFilePath);

};
