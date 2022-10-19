#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/WriteFileJob.h"

#include "WonderMakeBaseTests/JobMock.h"

class WriteFileJobMock
	: public JobMock<WriteFileJobMock>
{
public:
	MOCK_METHOD(void, RunFolderFilePathAndBuffer, (Promise<WriteFileJobMock::Output>, FilePath, std::vector<u8>));
	MOCK_METHOD(void, RunFolderFilePathAndString, (Promise<WriteFileJobMock::Output>, FilePath, std::string));

	inline void Run(Promise<WriteFileJobMock::Output> aPromise, FilePath aFilePath, std::vector<u8> aBuffer) override
	{
		RunFolderLocationAndPathAndBuffer(std::move(aPromise), std::move(aFilePath), std::move(aBuffer));
	}
	inline void Run(Promise<WriteFileJobMock::Output> aPromise, FilePath aFilePath, std::string aBuffer) override
	{
		RunFolderLocationAndPathAndString(std::move(aPromise), std::move(aFilePath), std::move(aBuffer));
	}
};
