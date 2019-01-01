#include "stdafx.h"
#include "ImGuiLogger.h"
#include "Imgui/imgui.h"


ImGuiLogger::ImGuiLogger()
	:mySubscriber(EThreadId::Logic, BindHelper(&ImGuiLogger::OnLogMessage, this))
{
}

void ImGuiLogger::Draw()
{
	ImGui::Begin("Console", 0);

	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	for (auto&& LogMessage : myLogMessages)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, LogMessage.Color);
		ImGui::TextUnformatted(LogMessage.Message.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleVar();

	ImGui::EndChild();

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

