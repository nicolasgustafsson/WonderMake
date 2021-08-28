#pragma once

#include "FileSystem/FileTypes.h"
#include "Job/Job.h"
#include "Utilities/Container/Container.h"

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
	ReadFileJob(const FolderLocation aLocation, const std::filesystem::path& aFilePath);

};
