#include "pch.h"
#include "ReadFileJob.h"

#include "FileSystem/FileSystem.h"

#include <fstream>

ReadFileJob::ReadFileJob(const FolderLocation aLocation, const std::filesystem::path& aFilePath)
{
	const auto dirPath = Get<FileSystem>().GetFolderLocation(aLocation);

	if (!dirPath)
	{
		CompleteFailure(ReadFileError::InvalidArguments);

		return;
	}

	const auto path = *dirPath / aFilePath;

	if (!std::filesystem::exists(path))
	{
		CompleteFailure(ReadFileError::FileNotFound);

		return;
	}

	if (!std::filesystem::is_regular_file(path))
	{
		CompleteFailure(ReadFileError::NotAFile);

		return;
	}

	std::ifstream file(path, std::ios_base::binary);

	if (!file)
	{
		CompleteFailure(ReadFileError::FailedToOpen);

		return;
	}

	Container<u8, Iterable, Indexable, ContiguousElements, EqualityComparable> buffer(std::istreambuf_iterator<char>(file), {});
	
	CompleteSuccess(std::move(buffer));
}
