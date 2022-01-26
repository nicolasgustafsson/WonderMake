#pragma once

#include "WonderMakeEngine/Logger.h"
#include "WonderMakeEngine/System.h"

#include <memory>

class WinPlatformSystem;

class WinLoggerConsoleSystem
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

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) override;

};
