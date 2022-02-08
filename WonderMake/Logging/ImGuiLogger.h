#pragma once

#include "Utilities/Debugging/Debugged.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

struct SImGuiLogMessage
{
	SColor Color;
	std::string Message;
};

class ImGuiLogger
	: public Debugged
	, public LoggerBase
	, public System<
		Policy::Set<>,
		STrait::Set<
			STGui,
			STLogger,
			STSingleton>>
	, public std::enable_shared_from_this<ImGuiLogger>
{
public:
	ImGuiLogger();

	void Initialize() override;

private:
	void Debug() override;

	void Print(ELogSeverity aSeverity, ELogLevel aLevel, std::string aLogMessage) override;

	std::vector<SImGuiLogMessage> myLogMessages;

	std::string myFilterText;

	bool myIsAtBottom = true;
	std::optional<f32> myPreviousScrollY;
};

