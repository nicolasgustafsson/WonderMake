#pragma once

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"

#include "wondermake-utility/Color.h"
#include "wondermake-utility/Guid.h"

#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <set>
#include <vector>

class DebugSettingsSystem;
class DebugSystem;

class ImGuiLogger
	: public LoggerBase
	, public System<
		Policy::Set<
			PAdd<DebugSettingsSystem, PWrite>,
			PAdd<DebugSystem, PWrite>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
	, public std::enable_shared_from_this<ImGuiLogger>
{
public:
	ImGuiLogger();

	void Initialize() override;

private:
	struct SImGuiLogLine
	{
		SColor						Color;
		std::string_view			Severity	= "Unset";
		std::string_view			Level		= "Unset";
		std::string					Message;
		std::string					File;
		std::string					FileWithLine;
		std::string					Timestamp;
		std::string					ThreadHash;
		std::string					LoggerName;
		std::string					ProcessId;
		std::vector<std::string>	LogTags;
	};
	struct SImGuiCompactLogLine
	{
		SImGuiLogLine	LogLine;
		u64				Count = 0;
	};
	struct SCategoryValues
	{
		std::set<std::string_view>	Severity;
		std::set<std::string_view>	Level;
		std::set<std::string>		File;
		std::set<std::string>		ThreadHash;
		std::set<std::string>		LoggerName;
		std::set<std::string>		ProcessId;
		std::set<std::string>		Tags;
	};

	struct SVisibleColumns
	{
		bool LoggerName	= false;
		bool ProcessId	= false;
		bool ThreadHash	= false;
		bool Timestamp	= false;
		bool Severity	= false;
		bool Level		= false;
		bool File		= false;
		bool MsgCount	= false;
		bool Message	= false;

		bool operator==(const SVisibleColumns&) const = default;
	};

	void Debug(bool& aIsOpen);
	template<bool CompactView>
	void UpdateLogs();

	void ReconstructCompactCache();
	void AddLogLineToCompactCache(const SImGuiLogLine& aLogLine);

	void Print(const SLogLine& aLogLine) override;

	std::mutex							myMutex;

	SCategoryValues						myCategoryValues;
	std::vector<SImGuiLogLine>			myLogMessages;
	std::vector<SImGuiCompactLogLine>	myCompactLogLinesCache;
	bool								myDirtyCompactCache = true;
	SVisibleColumns						myCurrentVisibleColumns;

	std::string		myFilterText;
	SCategoryValues	myFilterCategoryValues;
	bool			myCompactView = false;

	bool myIsAtBottom = true;
	std::optional<f32> myPreviousScrollY;
};

