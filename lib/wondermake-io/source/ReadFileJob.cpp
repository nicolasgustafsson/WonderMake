#include "wondermake-io/ReadFileJob.h"

#include "wondermake-base/JobGlobal.h"

#include <fstream>

class ReadFileJobImpl
	: public JobSub<>
	, public ReadFileJob
{
public:
	void Run(Promise<Result<std::vector<u8>, ReadFileError>> aPromise, FilePath aFilePath) override
	{
		if (!std::filesystem::exists(aFilePath))
		{
			aPromise.Complete(Err(ReadFileError::FileNotFound));

			return;
		}

		if (!std::filesystem::is_regular_file(aFilePath))
		{
			aPromise.Complete(Err(ReadFileError::NotAFile));

			return;
		}

		std::ifstream file(aFilePath, std::ios_base::binary);

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
