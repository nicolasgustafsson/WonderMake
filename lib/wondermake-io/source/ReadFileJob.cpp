#include "wondermake-io/ReadFileJob.h"

#include "wondermake-io/FileSystem.h"

#include "wondermake-base/jobs/JobGlobal.h"

#include <fstream>

WM_REGISTER_JOB(ReadFileJob);

void ReadFileJob::Run(Promise<Result<ReadFileError, std::vector<u8>>> aPromise, FolderLocation aLocation, std::filesystem::path aFilePath)
{
	const auto dirPath = Get<FileSystem>().GetFolderLocation(aLocation);

	if (!dirPath)
	{
		aPromise.Complete(ReadFileError::InvalidArguments);

		return;
	}

	const auto path = *dirPath / aFilePath;

	if (!std::filesystem::exists(path))
	{
		aPromise.Complete(ReadFileError::FileNotFound);

		return;
	}

	if (!std::filesystem::is_regular_file(path))
	{
		aPromise.Complete(ReadFileError::NotAFile);

		return;
	}

	std::ifstream file(path, std::ios_base::binary);

	if (!file)
	{
		aPromise.Complete(ReadFileError::FailedToOpen);

		return;
	}

	std::vector<u8> buffer(std::istreambuf_iterator<char>(file), {});
	
	aPromise.Complete(std::move(buffer));
}
