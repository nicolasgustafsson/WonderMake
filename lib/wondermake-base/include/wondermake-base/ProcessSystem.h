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

	virtual ProcessId GetCurrentProcessId() = 0;

	virtual Result<EStartError, std::shared_ptr<Process>> StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine) = 0;

};
