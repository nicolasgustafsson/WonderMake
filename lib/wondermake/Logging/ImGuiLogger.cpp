#include "pch.h"

#include "ImGuiLogger.h"

#include "wondermake-debug-ui/DebugSettingsSystem.h"
#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-utility/Time.h"

#include <sstream>
#include <unordered_map>

#include <magic_enum.hpp>

WM_REGISTER_SYSTEM(ImGuiLogger);

inline constexpr auto locCompactSettingName = "Logging/Compact";

template<typename TEnum>
std::unordered_map<TEnum, std::string> GetEnumNames()
{
	std::unordered_map<TEnum, std::string> retVal;

	for (auto [en, name] : magic_enum::enum_entries<TEnum>())
		retVal.emplace(std::make_pair(en, static_cast<std::string>(name)));

	return retVal;
}

static const auto locSeverityNames	= GetEnumNames<ELogSeverity>();
static const auto locLevelNames		= GetEnumNames<ELogLevel>();

ImGuiLogger::ImGuiLogger()
	: Debugged("Output Log")
{
	for (const auto& [en, name] : locSeverityNames)
		myCategoryValues.Severity.emplace(name);
	for (const auto& [en, name] : locLevelNames)
		myCategoryValues.Level.emplace(name);

	myFilterCategoryValues.Severity	= myCategoryValues.Severity;
	myFilterCategoryValues.Level	= myCategoryValues.Level;
}

void ImGuiLogger::Initialize()
{
	Logger::Get().AddLogger(weak_from_this());
}

void ImGuiLogger::Debug()
{
	myCompactView = Get<DebugSettingsSystem>().GetOrCreateDebugValue(locCompactSettingName, false);

	ImGui::Begin("Debug Log");

	if (myCompactView)
		UpdateLogs<true>();
	else
		UpdateLogs<false>();

	ImGui::Separator();

	ImGui::Text("Filter");
	
	ImGui::SameLine();
	
	ImGui::InputText("##", &myFilterText, ImGuiInputTextFlags_AutoSelectAll);

	ImGui::SameLine();

	if (ImGui::Button("Clear"))
	{
		myLogMessages.clear();
		myCompactLogLinesCache.clear();
		myDirtyCompactCache = false;

		myCategoryValues.File.clear();
		myCategoryValues.ThreadHash.clear();
		myCategoryValues.LoggerName.clear();
		myCategoryValues.ProcessId.clear();
		myCategoryValues.Tags.clear();

		myFilterCategoryValues = myCategoryValues;
	}

	ImGui::SameLine();

	if (ImGui::Checkbox("Compact", &myCompactView))
	{
		myDirtyCompactCache = true;

		Get<DebugSettingsSystem>().SetDebugValue(locCompactSettingName, myCompactView);
	}

	ImGui::End();
}

template<bool CompactView>
void ImGuiLogger::UpdateLogs()
{
	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	static constexpr int	columnCount	= CompactView ? 9 : 8;
	static constexpr auto	tableName	= CompactView ? "ConfigurationTableCompact" : "ConfigurationTable";

	if (ImGui::BeginTable(tableName, columnCount, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX, ImVec2(0, -footerHeight)))
	{
		int nextColumn			= 0;
		int columnLoggerName	= 0;
		int columnProcessId		= 0;
		int columnThreadHash	= 0;
		int columnTimestamp		= 0;
		int columnSeverity		= 0;
		int columnLevel			= 0;
		int columnFile			= 0;
		int columnMsgCount		= 0;
		int columnMessage		= 0;

		ImGui::TableSetupScrollFreeze(0, 1);

		ImGui::TableSetupColumn("Logger");
		columnLoggerName = nextColumn++;

		ImGui::TableSetupColumn("Process Id");
		columnProcessId = nextColumn++;

		ImGui::TableSetupColumn("Thread Hash", ImGuiTableColumnFlags_DefaultHide);
		columnThreadHash = nextColumn++;

		ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_DefaultHide);
		columnTimestamp = nextColumn++;

		ImGui::TableSetupColumn("Severity", ImGuiTableColumnFlags_DefaultHide);
		columnSeverity = nextColumn++;

		ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_DefaultHide);
		columnLevel = nextColumn++;

		ImGui::TableSetupColumn("File");
		columnFile = nextColumn++;

		if constexpr (CompactView)
		{
			ImGui::TableSetupColumn("Count");
			columnMsgCount = nextColumn++;
		}

		ImGui::TableSetupColumn("Message");
		columnMessage = nextColumn++;

		ImGui::TableHeadersRow();

		SVisibleColumns visibleColumns;
		
		visibleColumns.LoggerName		= (ImGui::TableGetColumnFlags(columnLoggerName)	& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.ProcessId		= (ImGui::TableGetColumnFlags(columnProcessId)	& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.ThreadHash		= (ImGui::TableGetColumnFlags(columnThreadHash)	& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.Timestamp		= (ImGui::TableGetColumnFlags(columnTimestamp)	& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.Severity			= (ImGui::TableGetColumnFlags(columnSeverity)	& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.Level			= (ImGui::TableGetColumnFlags(columnLevel)		& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.File				= (ImGui::TableGetColumnFlags(columnFile)		& ImGuiTableColumnFlags_IsEnabled) != 0;
		if constexpr (CompactView)
			visibleColumns.MsgCount		= (ImGui::TableGetColumnFlags(columnMsgCount)	& ImGuiTableColumnFlags_IsEnabled) != 0;
		visibleColumns.Message			= (ImGui::TableGetColumnFlags(columnMessage)	& ImGuiTableColumnFlags_IsEnabled) != 0;

		if constexpr (CompactView)
			if (myCurrentVisibleColumns != visibleColumns)
				myDirtyCompactCache = true;

		myCurrentVisibleColumns = visibleColumns;

		if constexpr (CompactView)
			if (myDirtyCompactCache)
				ReconstructCompactCache();

		static constexpr auto getLogLine = [](const auto& aLogLine) -> const auto&
		{
			if constexpr (CompactView)
				return aLogLine.LogLine;
			else
				return aLogLine;
		};

		const auto logFilter = [&visibleColumns = myCurrentVisibleColumns, &filterText = myFilterText, &filterCategory = myFilterCategoryValues](const auto& aLogLine) -> bool
		{
			const auto& logLine = getLogLine(aLogLine);

			if (!filterText.empty())
			{
				const auto filter = [&filterText](const auto& aText) -> bool
				{
					auto searchResult = std::search(
						aText.begin(), aText.end(),
						filterText.begin(), filterText.end(),
						[](char a, char b)
						{
							return std::tolower(a) == std::tolower(b);
						});

					return searchResult != aText.end();
				};

				bool notFiltered = false;

				notFiltered |= (visibleColumns.LoggerName		&& filter(logLine.LoggerName));
				notFiltered |= (visibleColumns.ProcessId		&& filter(logLine.ProcessId));
				notFiltered |= (visibleColumns.ThreadHash		&& filter(logLine.ThreadHash));
				notFiltered |= (visibleColumns.Timestamp		&& filter(logLine.Timestamp));
				notFiltered |= (visibleColumns.Severity			&& filter(std::string_view(logLine.Severity)));
				notFiltered |= (visibleColumns.Level			&& filter(std::string_view(logLine.Level)));
				notFiltered |= (visibleColumns.File				&& filter(logLine.FileWithLine));
				notFiltered |= (visibleColumns.Message			&& filter(logLine.Message));

				if (!notFiltered)
					return false;
			}

			const auto predHasTag = [&tagsFilter = filterCategory.Tags](const auto& aTag)
			{
				return tagsFilter.contains(aTag);
			};

			if (visibleColumns.LoggerName		&& !filterCategory.LoggerName.contains(logLine.LoggerName))
				return false;
			if (visibleColumns.ProcessId		&& !filterCategory.ProcessId.contains(logLine.ProcessId))
				return false;
			if (visibleColumns.ThreadHash		&& !filterCategory.ThreadHash.contains(logLine.ThreadHash))
				return false;
			if (visibleColumns.Severity			&& !filterCategory.Severity.contains(logLine.Severity))
				return false;
			if (visibleColumns.Level			&& !filterCategory.Level.contains(logLine.Level))
				return false;
			if (visibleColumns.File				&& !filterCategory.File.contains(logLine.File))
				return false;
			if (visibleColumns.Message			&& std::ranges::none_of(logLine.LogTags, predHasTag))
				return false;

			return true;
		};
		const auto getLogLines = [this]() -> auto&
		{
			if constexpr (CompactView)
				return myCompactLogLinesCache;
			else
				return myLogMessages;
		};

		for (const auto& logLineData : getLogLines()
								 | std::views::filter(logFilter))
		{
			const auto& logLine = getLogLine(logLineData);

			ImGui::TableNextRow();

			ImGui::PushStyleColor(ImGuiCol_Text, logLine.Color);

			ImGui::TableSetColumnIndex(columnLoggerName);

			ImGui::Text("%s", logLine.LoggerName.c_str());

			ImGui::TableSetColumnIndex(columnProcessId);

			ImGui::Text("%s", logLine.ProcessId.c_str());

			ImGui::TableSetColumnIndex(columnThreadHash);

			ImGui::Text("%s", logLine.ThreadHash.c_str());

			ImGui::TableSetColumnIndex(columnTimestamp);

			ImGui::Text("%s", logLine.Timestamp.c_str());

			ImGui::TableSetColumnIndex(columnSeverity);

			ImGui::Text("%s", logLine.Severity.data());

			ImGui::TableSetColumnIndex(columnLevel);

			ImGui::Text("%s", logLine.Level.data());

			ImGui::TableSetColumnIndex(columnFile);

			ImGui::Text("%s", logLine.FileWithLine.c_str());

			if constexpr (CompactView)
			{
				ImGui::TableSetColumnIndex(columnMsgCount);
				static std::string countText = "0";

				countText = std::to_string(logLineData.Count);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX()
					+ ImGui::GetColumnWidth() - ImGui::CalcTextSize(countText.c_str()).x
					- ImGui::GetScrollX());

				ImGui::Text("%s", countText.c_str());
			}

			ImGui::TableSetColumnIndex(columnMessage);

			ImGui::Text("%s", logLine.Message.c_str());

			ImGui::PopStyleColor();
		}

		static const auto columsWithPopups = std::array<int, 7>({ columnLoggerName, columnProcessId, columnThreadHash, columnSeverity, columnLevel, columnFile, columnMessage });

		static constexpr auto beginPopup = [](const auto& aCategories, auto& aFilter)
		{
			bool checkedAll = aCategories == aFilter;
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			if (ImGui::MenuItem("All", "", &checkedAll))
			{
				if (checkedAll)
					aFilter = aCategories;
				else
					aFilter.clear();
			}

			ImGui::Separator();

			for (const auto& category : aCategories)
			{
				bool checked = aFilter.contains(category) || checkedAll;

				if (ImGui::MenuItem(category.data(), "", &checked))
				{
					if (ImGui::GetIO().KeyShift)
						aFilter = { category };
					else if (ImGui::GetIO().KeyCtrl)
					{
						aFilter = aCategories;

						aFilter.erase(category);
					}
					else if (checked)
						aFilter.emplace(category);
					else
						aFilter.erase(category);
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.f);

					ImGui::TextUnformatted("Shift-click to select only this.");

					ImGui::TextUnformatted("Ctrl-click to select all but this.");

					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
			}

			ImGui::PopItemFlag();
		};

		for (int column : columsWithPopups)
		{
			ImGui::PushID(column);

			ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, 0.f), ImVec2(500.f, 250.f));

			if ((ImGui::TableGetColumnFlags(column) & ImGuiTableColumnFlags_IsHovered) && !ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				ImGui::OpenPopup("CategoryFilterPopup");

			if (ImGui::BeginPopup("CategoryFilterPopup"))
			{
				if (column == columnLoggerName)
					beginPopup(myCategoryValues.LoggerName, myFilterCategoryValues.LoggerName);
				else if (column == columnProcessId)
					beginPopup(myCategoryValues.ProcessId, myFilterCategoryValues.ProcessId);
				else if (column == columnThreadHash)
					beginPopup(myCategoryValues.ThreadHash, myFilterCategoryValues.ThreadHash);
				else if (column == columnSeverity)
					beginPopup(myCategoryValues.Severity, myFilterCategoryValues.Severity);
				else if (column == columnLevel)
					beginPopup(myCategoryValues.Level, myFilterCategoryValues.Level);
				else if (column == columnFile)
					beginPopup(myCategoryValues.File, myFilterCategoryValues.File);
				else if (column == columnMessage)
					beginPopup(myCategoryValues.Tags, myFilterCategoryValues.Tags);

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		const bool hasScrolled = myPreviousScrollY && ImGui::GetScrollY() != myPreviousScrollY;

		const bool shouldScrollToBottom = (!hasScrolled && myIsAtBottom);
		if (shouldScrollToBottom)
		{
			ImGui::SetScrollY(ImGui::GetScrollMaxY());
			myPreviousScrollY = ImGui::GetScrollMaxY();
		}
		else
			myPreviousScrollY = ImGui::GetScrollY();

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		myIsAtBottom = ImGui::GetScrollMaxY() == ImGui::GetScrollY() || ImGui::GetScrollMaxY() == window->ScrollTarget.y;

		ImGui::EndTable();
	}
}

void ImGuiLogger::ReconstructCompactCache()
{
	static constexpr auto getLogLine = [](const auto& aLogLine) -> const auto&
	{
		return aLogLine;
	};

	myCompactLogLinesCache.clear();

	for (const auto& logLine : myLogMessages)
		AddLogLineToCompactCache(logLine);

	myDirtyCompactCache = false;
}

void ImGuiLogger::AddLogLineToCompactCache(const SImGuiLogLine& aLogLine)
{
	const auto findPred = [&visibleColumns = myCurrentVisibleColumns, &filterCategory = myFilterCategoryValues, &aLogLine](const auto& aCompactLogLineData) -> bool
	{
		if (visibleColumns.LoggerName	&& aLogLine.LoggerName	!= aCompactLogLineData.LogLine.LoggerName)
			return false;
		if (visibleColumns.Severity		&& aLogLine.Severity	!= aCompactLogLineData.LogLine.Severity)
			return false;
		if (visibleColumns.Level		&& aLogLine.Level		!= aCompactLogLineData.LogLine.Level)
			return false;
		if (visibleColumns.File			&& aLogLine.File		!= aCompactLogLineData.LogLine.File)
			return false;
		if (visibleColumns.Message		&& aLogLine.Message		!= aCompactLogLineData.LogLine.Message)
			return false;

		return true;
	};

	const auto it = std::ranges::find_if(myCompactLogLinesCache, findPred);

	if (it == myCompactLogLinesCache.end())
		myCompactLogLinesCache.emplace_back(SImGuiCompactLogLine
			{
				aLogLine,
				1
			});
	else
		++it->Count;
}

void ImGuiLogger::Print(const SLogLine& aLogLine)
{
	static constexpr auto getColor = [](ELogSeverity aSeverity)
	{
		switch (aSeverity)
		{
		case ELogSeverity::Success:	return SColor::Green();
		case ELogSeverity::Info:	return SColor::White();
		case ELogSeverity::Warning:	return SColor::Yellow();
		case ELogSeverity::Error:	return SColor::Red();
		}

		return SColor::White();
	};
	static constexpr auto getName = [](const auto& aMap, const auto aKey) -> const char*
	{
		if (auto it = aMap.find(aKey); it != aMap.end())
			return it->second.c_str();

		return "Unknown";
	};

	std::stringstream threadHashStream;

	threadHashStream << "[0x" << std::hex << aLogLine.ThreadHash << ']';

	auto nameSeverity	= getName(locSeverityNames,	aLogLine.Severity);
	auto nameLevel		= getName(locLevelNames,	aLogLine.Level);
	auto fileWithLine	= aLogLine.File + '(' + std::to_string(aLogLine.Line) + ')';
	auto threadHash		= std::move(threadHashStream).str();
	auto tags			= ExtractLogTags(aLogLine.Message);

	const bool allFile			= myCategoryValues.File			== myFilterCategoryValues.File;
	const bool allThreadHash	= myCategoryValues.ThreadHash	== myFilterCategoryValues.ThreadHash;
	const bool allLoggerName	= myCategoryValues.LoggerName	== myFilterCategoryValues.LoggerName;
	const bool allProcessId		= myCategoryValues.ProcessId	== myFilterCategoryValues.ProcessId;
	const bool allTags			= myCategoryValues.Tags			== myFilterCategoryValues.Tags;

	myCategoryValues.File		.emplace(aLogLine.File);
	myCategoryValues.ThreadHash	.emplace(threadHash);
	myCategoryValues.LoggerName	.emplace(aLogLine.LoggerName);
	myCategoryValues.ProcessId	.emplace(static_cast<std::string>(aLogLine.ProcessId.Id()));
	myCategoryValues.Tags		.insert(tags.begin(), tags.end());

	if (allFile)		myFilterCategoryValues.File			= myCategoryValues.File;
	if (allThreadHash)	myFilterCategoryValues.ThreadHash	= myCategoryValues.ThreadHash;
	if (allLoggerName)	myFilterCategoryValues.LoggerName	= myCategoryValues.LoggerName;
	if (allProcessId)	myFilterCategoryValues.ProcessId	= myCategoryValues.ProcessId;
	if (allTags)		myFilterCategoryValues.Tags			= myCategoryValues.Tags;

	SImGuiLogLine logLine
	{
		getColor(aLogLine.Severity),
		nameSeverity,
		nameLevel,
		aLogLine.Message,
		aLogLine.File,
		std::move(fileWithLine),
		aLogLine.Timestamp,
		std::move(threadHash),
		aLogLine.LoggerName,
		static_cast<std::string>(aLogLine.ProcessId.Id()),
		std::move(tags)
	};
	
	if (!myDirtyCompactCache)
		AddLogLineToCompactCache(logLine);

	myLogMessages.emplace_back(std::move(logLine));
}

std::vector<std::string> ImGuiLogger::ExtractLogTags(std::string_view aMessage)
{
	std::vector<std::string> retVal;

	if (aMessage.empty()
		|| aMessage[0] != '[')
		return retVal;
	
	size_t index = 0;

	for (;;)
	{
		auto i = aMessage.find_first_of(']', index);

		if (i == std::string_view::npos)
			break;

		retVal.emplace_back(std::string(aMessage.begin() + index, aMessage.begin() + i + 1));

		if (aMessage.size() <= i + 2
			|| aMessage[i + 1] != ' '
			|| aMessage[i + 2] != '[')
			break;

		index = i + 2;
	}

	return retVal;
}
