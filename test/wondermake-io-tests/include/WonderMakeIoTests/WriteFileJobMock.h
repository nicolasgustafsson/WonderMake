#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-io/WriteFileJob.h"

#include "WonderMakeBaseTests/JobMock.h"

class WriteFileJobMock
	: public JobMock<WriteFileJobMock>
{
public:
	MOCK_METHOD(void, RunFolderLocationAndPathAndBuffer, (Promise<WriteFileJobMock::Output>, FolderLocation, std::filesystem::path, std::vector<u8>));
	MOCK_METHOD(void, RunFolderLocationAndPathAndString, (Promise<WriteFileJobMock::Output>, FolderLocation, std::filesystem::path, std::string));

	inline void Run(Promise<WriteFileJobMock::Output> aPromise, FolderLocation aLocation, std::filesystem::path aPath, std::vector<u8> aBuffer) override
	{
		RunFolderLocationAndPathAndBuffer(std::move(aPromise), std::move(aLocation), std::move(aPath), std::move(aBuffer));
	}
	inline void Run(Promise<WriteFileJobMock::Output> aPromise, FolderLocation aLocation, std::filesystem::path aPath, std::string aBuffer) override
	{
		RunFolderLocationAndPathAndString(std::move(aPromise), std::move(aLocation), std::move(aPath), std::move(aBuffer));
	}
};
