#include "wondermake-io/WriteFileJob.h"

#include "wondermake-io/FileSystem.h"

#include "wondermake-base/JobGlobal.h"

#include <fstream>

class WriteFileJobImpl
	: public JobSub<
		Policy::Set<
			PAdd<FileSystem, PWrite>>>
	, public WriteFileJob
{
public:
	void Run(Promise<Output> aPromise, FolderLocation aLocation, std::filesystem::path aFilePath, std::vector<u8> aBuffer) override
	{
		std::filesystem::path path;

		if (aFilePath.is_relative())
		{
			const auto dirPath = Get<FileSystem>().GetFolderLocation(aLocation);

			if (!dirPath)
			{
				aPromise.Complete(Err(EWriteFileError::InvalidArguments));

				return;
			}

			path = *dirPath / aFilePath;
		}
		else
		{
			path = std::move(aFilePath);
		}

		if (std::filesystem::is_directory(path))
		{
			aPromise.Complete(Err(EWriteFileError::PathIsAlreadyADirectory));

			return;
		}

		if (!std::filesystem::is_directory(path.parent_path())
			&& !std::filesystem::create_directories(path.parent_path()))
		{
			aPromise.Complete(Err(EWriteFileError::FailedToCreateDirectory));

			return;
		}

		std::ofstream file(path, std::ios_base::binary);

		if (!file)
		{
			aPromise.Complete(Err(EWriteFileError::FailedToOpen));

			return;
		}

		file.write(reinterpret_cast<const char*>(aBuffer.data()), aBuffer.size());;
	
		file.flush();

		file.close();

		aPromise.Complete(Ok());
	}
	void Run(Promise<Output> aPromise, FolderLocation aLocation, std::filesystem::path aFilePath, std::string aBuffer) override
	{
		Run(std::move(aPromise), aLocation, std::move(aFilePath), std::vector<u8>(aBuffer.begin(), aBuffer.end()));
	}

};

WM_REGISTER_JOB_MASKED(WriteFileJobImpl, WriteFileJob);
