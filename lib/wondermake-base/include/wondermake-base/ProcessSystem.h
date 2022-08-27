#pragma once

#include "wondermake-base/ProcessId.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Result.h"
#include "wondermake-utility/UniqueFunction.h"

#include <filesystem>
#include <memory>

class Process;

class ProcessSystem
	: public SystemAbstracted
{
public:
	enum class EStartError
	{
		FileNotFound,
		InternalError
	};

	struct SError
	{
		EStartError Error = EStartError::InternalError;
		u64 Reason = 0;
	};

	virtual ProcessId GetCurrentProcessId() = 0;

	virtual Result<std::shared_ptr<Process>, SError> StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine) = 0;

};
