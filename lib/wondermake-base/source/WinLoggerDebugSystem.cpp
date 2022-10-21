#include "WinLoggerDebugSystem.h"

#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WinPlatformSystem.h"

#include "wondermake-utility/StringUtility.h"

REGISTER_SYSTEM(WinLoggerDebugSystem);

void WinLoggerDebugSystem::Initialize()
{
	Logger::Get().AddLogger(weak_from_this());
}

void WinLoggerDebugSystem::Print(const SLogLine& aLogLine)
{
	auto formattedLine = Logger::FormatLine(aLogLine);

	std::wstring wMessage(formattedLine.size() + 1, L'0');

	wMessage = Utility::Utf8Decode(formattedLine);

	wMessage = wMessage + L'\n';

    Get<WinPlatformSystem>().OutputDebugStringW(wMessage.c_str());
}
