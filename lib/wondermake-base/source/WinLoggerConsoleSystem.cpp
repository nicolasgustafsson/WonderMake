#include "WinLoggerConsoleSystem.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/WinPlatform.h"

#include <iostream>
#include <sstream>

REGISTER_SYSTEM(WinLoggerConsoleSystem);

void WinLoggerConsoleSystem::Initialize()
{
	Logger::Get().AddLogger(weak_from_this());
}

void WinLoggerConsoleSystem::Print(ELogSeverity aSeverity, ELogLevel /*aLevel*/, std::string aLogMessage)
{
	auto&& winPlatform = Get<WinPlatformSystem>();

	const auto hConsole = winPlatform.GetStdHandle(STD_OUTPUT_HANDLE);

	WORD attribute = 7;

	switch (aSeverity)
	{
	case ELogSeverity::Success:
		attribute = 10;
		break;
	case ELogSeverity::Warning:
		attribute = 14;
		break;
	case ELogSeverity::Error:
		attribute = 12;
		break;
	case ELogSeverity::Info:
		attribute = 7;
		break;
	}

	(void)winPlatform.SetConsoleTextAttribute(hConsole, attribute);

	std::cout << aLogMessage << std::endl;
}
