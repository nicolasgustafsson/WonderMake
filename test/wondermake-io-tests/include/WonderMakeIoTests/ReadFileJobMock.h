#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/ReadFileJob.h"

#include "WonderMakeBaseTests/JobMock.h"

class ReadFileJobMock
	: public JobMock<ReadFileJob>
{
public:
	MOCK_METHOD(void, RunFolderLocationAndPath, (Promise<ReadFileJob::Output>, FolderLocation, std::filesystem::path));

	inline void Run(Promise<ReadFileJob::Output> aPromise, FolderLocation aLocation, std::filesystem::path aPath) override
	{
		RunFolderLocationAndPath(std::move(aPromise), std::move(aLocation), std::move(aPath));
	}
};
