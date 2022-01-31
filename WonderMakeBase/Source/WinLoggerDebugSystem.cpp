#include "WinLoggerDebugSystem.h"

#include "WonderMakeBase/SystemGlobal.h"
#include "WonderMakeBase/WinPlatformSystem.h"

#include "WonderMakeUtility/StringUtility.h"

REGISTER_SYSTEM(WinLoggerDebugSystem);

void WinLoggerDebugSystem::Initialize()
{
	Logger::Get().AddLogger(shared_from_this());
}

void WinLoggerDebugSystem::Print(ELogSeverity /*aSeverity*/, ELogLevel /*aLevel*/, std::string aLogMessage)
{
	std::wstring wMessage(aLogMessage.size() + 1, L'0');

	wMessage = Utility::Utf8Decode(aLogMessage);

	wMessage = wMessage + L'\n';

    Get<WinPlatformSystem>().OutputDebugStringW(wMessage.c_str());
}
