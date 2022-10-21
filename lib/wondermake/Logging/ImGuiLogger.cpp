#include "pch.h"

#include "ImGuiLogger.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-utility/Time.h"

#include <sstream>

REGISTER_SYSTEM(ImGuiLogger);

ImGuiLogger::ImGuiLogger()
	: Debugged("Output Log")
{
}

void ImGuiLogger::Initialize()
{
	Logger::Get().AddLogger(weak_from_this());
}

void ImGuiLogger::Debug()
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

	ImGui::Begin("Debug Log", 0);

	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	for (auto&& logMessage : myLogMessages)
	{
		 // Skip filtered messages
		if (!myFilterText.empty())
		{
			auto searchResult = std::search(
				logMessage.Message.begin(), logMessage.Message.end(),
				myFilterText.begin(), myFilterText.end(),
				[](char a, char b) {return std::tolower(a) == std::tolower(b); }
			);

			if (searchResult == logMessage.Message.end())
				continue;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, getColor(logMessage.Severity));
		ImGui::TextUnformatted(logMessage.Message.c_str());
		ImGui::PopStyleColor();
	}

	const bool hasScrolled = myPreviousScrollY && ImGui::GetScrollY() != myPreviousScrollY;
	
	const bool shouldScrollToBottom = (!hasScrolled && myIsAtBottom);
	if (shouldScrollToBottom)
	{
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
		myPreviousScrollY = ImGui::GetScrollMaxY();
	}
	else
	{
		myPreviousScrollY = ImGui::GetScrollY();
	}

	ImGuiWindow* window = ImGui::GetCurrentWindow();

	myIsAtBottom = ImGui::GetScrollMaxY() == ImGui::GetScrollY() || ImGui::GetScrollMaxY() == window->ScrollTarget.y;

	ImGui::PopStyleVar();

	ImGui::EndChild();

	ImGui::Separator();

	ImGui::Text("Filter");
	ImGui::SameLine();
	ImGui::InputText("Write to filter", &myFilterText, ImGuiInputTextFlags_AutoSelectAll);

	ImGui::End();
}

void ImGuiLogger::Print(const SLogLine& aLogLine)
{
	myLogMessages.emplace_back(aLogLine);
}

