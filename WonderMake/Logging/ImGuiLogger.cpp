#include "pch.h"
#include "ImGuiLogger.h"
#include "Imgui/ImguiInclude.h"
#include <cctype>
#include <iostream>

ImGuiLogger::ImGuiLogger()
	: mySubscriber(ERoutineId::Logic, BindHelper(&ImGuiLogger::OnLogMessage, this))
	, Debugged("Output Log")
{
}

void ImGuiLogger::Debug()
{
	ImGui::Begin("Debug Log", 0);

	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing


	for (auto&& logMessage : myLogMessages)
	{
		//skip filtered messages
		if (!myFilterText.empty())
		{
			auto searchResult = std::search(
				logMessage.Message.begin(), logMessage.Message.end(),
				myFilterText.begin(), myFilterText.end(),
				[](char a, char b) {return std::tolower(a) == std::tolower(b); }
			);

			if (searchResult == logMessage.Message.end())
			{
				continue;
			}
		}

		ImGui::PushStyleColor(ImGuiCol_Text, logMessage.Color);
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

void ImGuiLogger::OnLogMessage(const SLogMessage& aMessage)
{
	SColor color = SColor::White;
	if (aMessage.HasTag(TagError))
		color = SColor::Red;
	if (aMessage.HasTag(TagWarning))
		color = SColor::Yellow;
	if (aMessage.HasTag(TagSuccess))
		color = SColor::Green;

	myLogMessages.push_back({ color, std::move(aMessage.LogText) });
}

