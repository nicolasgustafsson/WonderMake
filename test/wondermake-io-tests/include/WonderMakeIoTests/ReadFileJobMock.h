#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/ReadFileJob.h"

#include "WonderMakeBaseTests/JobMock.h"

class ReadFileJobMock
	: public JobMock<ReadFileJob>
{
public:
	MOCK_METHOD(void, RunFolderFilePath, (Promise<ReadFileJob::Output>, FilePath));

	inline void Run(Promise<ReadFileJob::Output> aPromise, FilePath aFilePath) override
	{
		RunFolderFilePath(std::move(aPromise), std::move(aFilePath));
	}
};
