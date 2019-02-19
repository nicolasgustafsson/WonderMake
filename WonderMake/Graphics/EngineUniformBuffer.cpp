#include "stdafx.h"
#include "EngineUniformBuffer.h"

EngineUniformBuffer::EngineUniformBuffer()
	: UniformBuffer<SEngineUniformBufferData>(0)
	, mySubscriber(ERoutineId::Debug, BindHelper(&EngineUniformBuffer::Debug, this))
{

}

void EngineUniformBuffer::Debug(const SDebugMessage&)
{
	ImGui::Begin("Engine Uniform Buffer info");

	ImGui::Separator();

	ImGui::Text("Projection Matrix");
	ImGui::TextUnformatted(GetBuffer().ProjectionMatrix.ToString().c_str());

	ImGui::Separator();

	ImGui::Text("View Matrix");
	ImGui::TextUnformatted(GetBuffer().ViewMatrix.ToString().c_str());

	ImGui::Separator();

	ImGui::Text("View Projection Matrix");
	ImGui::TextUnformatted(GetBuffer().ViewProjectionMatrix.ToString().c_str());

	ImGui::Separator();

	std::string timeString = "Time: " + std::to_string(GetBuffer().Time);
	ImGui::Text(timeString.c_str());

	ImGui::Separator();

	ImGui::End();
	
}
