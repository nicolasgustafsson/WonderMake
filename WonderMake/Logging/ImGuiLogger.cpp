#include "stdafx.h"
#include "ImGuiLogger.h"
#include "Imgui/imgui.h"
#include "Imgui/imgui_stdlib.h"
#include <cctype>

ImGuiLogger::ImGuiLogger()
	: mySubscriber(ERoutineId::Logic, BindHelper(&ImGuiLogger::OnLogMessage, this))
	, myDebugSubscriber(ERoutineId::Debug, BindHelper(&ImGuiLogger::Debug, this))
{
}

void ImGuiLogger::Debug(const SDebugMessage&)
{
	ImGui::Begin("Console", 0);

	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	for (auto&& LogMessage : myLogMessages)
	{
		//skip filtered messages
		if (!myFilterText.empty())
		{
			auto searchResult = std::search(
				LogMessage.Message.begin(), LogMessage.Message.end(),
				myFilterText.begin(), myFilterText.end(),
				[](char a, char b) {return std::tolower(a) == std::tolower(b);}
			);

			if (searchResult == LogMessage.Message.end())
			{
				continue;
			}
		}

		ImGui::PushStyleColor(ImGuiCol_Text, LogMessage.Color);
		ImGui::TextUnformatted(LogMessage.Message.c_str());
		ImGui::PopStyleColor();
	}

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

	myLogMessages.push_back({ color, std::move(aMessage.LogText) });
}

