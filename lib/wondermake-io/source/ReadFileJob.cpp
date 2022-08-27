#include "wondermake-io/ReadFileJob.h"

#include "wondermake-io/FileSystem.h"

#include "wondermake-base/JobGlobal.h"

#include <fstream>

class ReadFileJobImpl
	: public JobSub<
		Policy::Set<
			PAdd<FileSystem, PWrite>>>
	, public ReadFileJob
{
public:
	void Run(Promise<Result<std::vector<u8>, ReadFileError>> aPromise, FolderLocation aLocation, std::filesystem::path aFilePath) override
	{
		std::filesystem::path path;

		if (aFilePath.is_relative())
		{
			const auto dirPath = Get<FileSystem>().GetFolderLocation(aLocation);

			if (!dirPath)
			{
				aPromise.Complete(Err(ReadFileError::InvalidArguments));

				return;
			}

			path = *dirPath / aFilePath;
		}
		else
		{
			path = std::move(aFilePath);
		}

		if (!std::filesystem::exists(path))
		{
			aPromise.Complete(Err(ReadFileError::FileNotFound));

			return;
		}

		if (!std::filesystem::is_regular_file(path))
		{
			aPromise.Complete(Err(ReadFileError::NotAFile));

			return;
		}

		std::ifstream file(path, std::ios_base::binary);

		if (!file)
		{
			aPromise.Complete(Err(ReadFileError::FailedToOpen));

			return;
		}

		std::vector<u8> buffer(std::istreambuf_iterator<char>(file), {});
	
		aPromise.Complete(Ok(std::move(buffer)));
	}

};

WM_REGISTER_JOB_MASKED(ReadFileJobImpl, ReadFileJob);
