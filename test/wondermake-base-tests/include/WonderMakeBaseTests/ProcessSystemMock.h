#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/ProcessSystem.h"

class ProcessSystemMock
	: public ProcessSystem
{
public:
	MOCK_METHOD(ProcessId, GetCurrentProcessId, (), (override));

	using StartProcessRet = Result<std::shared_ptr<Process>, SStartError>;
	MOCK_METHOD(StartProcessRet, StartProcess, (std::filesystem::path aApplicationPath, std::wstring aCommandLine), (override));

};
