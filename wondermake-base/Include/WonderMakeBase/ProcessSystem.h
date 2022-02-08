#pragma once

#include "WonderMakeBase/System.h"

#include "WonderMakeUtility/Result.h"
#include "WonderMakeUtility/UniqueFunction.h"

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

	virtual Result<EStartError, std::shared_ptr<Process>> StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine) = 0;

};
