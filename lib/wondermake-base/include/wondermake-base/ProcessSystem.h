#pragma once

#include "wondermake-base/ProcessId.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Result.h"
#include "wondermake-utility/UniqueFunction.h"

#include <magic_enum.hpp>

#include <filesystem>
#include <memory>
#include <ostream>

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

	struct SStartError
	{
		EStartError Error = EStartError::InternalError;
		u64 Reason = 0;
	};

	virtual ProcessId GetCurrentProcessId() = 0;

	virtual Result<std::shared_ptr<Process>, SStartError> StartProcess(std::filesystem::path aApplicationPath, std::wstring aCommandLine) = 0;

};

inline static void WmLogStream(std::ostream& aStream, const ProcessSystem::SStartError& aError)
{
	aStream << magic_enum::enum_name(aError.Error) << '(' << static_cast<std::underlying_type_t<decltype(aError.Error)>>(aError.Error) << ':' << aError.Reason << ')';
}
