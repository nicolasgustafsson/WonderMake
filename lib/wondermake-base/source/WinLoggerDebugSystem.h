#pragma once

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include <memory>

class WinPlatformSystem;

class WinLoggerDebugSystem
	: public System<
		Policy::Set<
			PAdd<WinPlatformSystem, PWrite>>,
		SystemTraits::Set<
			STLogger,
			STThreadsafe,
			STSingleton>>
	, public LoggerBase
	, public std::enable_shared_from_this<LoggerBase>
{
public:
	void Initialize() override;

	void Print(const SLogLine& aLogLine) override;

};
