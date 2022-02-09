#pragma once
#include "Imgui/FileSelector.h"
#include "Graphics/RenderSettingsManager.h"
#include "Imgui/imgui.h"
#include "Utilities/Color.h"
#include "Utilities/Vector.h"

class NodeGraph;

enum class EAlterStatus : bool
{
	Same = false,
	Changed = true
};

inline EAlterStatus operator||(const EAlterStatus& aFirst, const EAlterStatus& aSecond)
{
	if (aFirst == EAlterStatus::Changed || aSecond == EAlterStatus::Changed)
		return EAlterStatus::Changed;

	return EAlterStatus::Same;
}

/*
inline EAlterStatus& operator++(EAlterStatus& aAlterStatus) {
	if (aAlterStatus == EAlterStatus::Same)
	{
		aAlterStatus = EAlterStatus::Changed;
	}

	//Nicos: The more things change, the more they stay the same
	if (aAlterStatus == EAlterStatus::Changed)
	{
		aAlterStatus = EAlterStatus::Same;
	}

	return aAlterStatus;
}
*/

namespace SlotInputEdits
{
	template<typename TSlotType>
	EAlterStatus EditInputSlot(TSlotType&)
	{
		return EAlterStatus::Same;
	}

	template<typename TSlotType>
	EAlterStatus EditNodeGraphInputSlot(TSlotType& aSlotType, NodeGraph&)
	{
		return EditInputSlot(aSlotType);
	}

	template<typename TSlotType> requires requires (TSlotType x, NodeGraph& aNodeGraph) { x.NodeGraphInspect(aNodeGraph); }
	EAlterStatus EditNodeGraphInputSlot(TSlotType& aSlotType, NodeGraph& aNodeGraph)
	{
		return aSlotType.NodeGraphInspect(aNodeGraph) ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<float>(float& aInput)
	{
		return ImGui::InputFloat("", &aInput) ? EAlterStatus::Changed : EAlterStatus::Same;
	} 

	template<>
	inline EAlterStatus EditInputSlot<int>(int& aInput)
	{
		return ImGui::InputInt("", &aInput) ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<std::string>(std::string& aInput)
	{
		return ImGui::InputText("", &aInput) ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<bool>(bool& aInput)
	{
		return ImGui::Checkbox("", &aInput) ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<SVector2u>(SVector2u& aInput)
	{
		i32 inputs[]{ static_cast<i32>(aInput.X), static_cast<i32>(aInput.Y)};
		const bool changed = ImGui::InputInt2("", inputs);

		aInput.X = static_cast<u32>(inputs[0]);
		aInput.Y = static_cast<u32>(inputs[1]);

		return changed ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<SVector2f>(SVector2f& aInput)
	{
		f32 inputs[]{ static_cast<f32>(aInput.X), static_cast<f32>(aInput.Y) };
		const bool changed = ImGui::InputFloat2("", inputs);

		aInput.X = (inputs[0]);
		aInput.Y = (inputs[1]);

		return changed ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<SColor>(SColor& aInput)
	{
		return ImGui::ColorEdit4("", ((f32*)&aInput)) ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<std::filesystem::path>(std::filesystem::path& aInput)
	{
		return ImGui::FileSelector::SelectFile(aInput) ? EAlterStatus::Changed : EAlterStatus::Same;
	}

	template<>
	inline EAlterStatus EditInputSlot<SRenderSettings>(SRenderSettings& aInput)
	{
		SRenderSettings initial = aInput;
		ImGui::PushID(&aInput);
		i32 blendModeIndex = 0;

		if (aInput.BlendMode)
			blendModeIndex = static_cast<i32>(*aInput.BlendMode) + 1;
			
		if (ImGui::Combo("Blend Mode", &blendModeIndex, "Current\0Alpha\0Additive\0"))
		{
			switch (blendModeIndex)
			{
			case 0: aInput.BlendMode.reset(); break;
			default: aInput.BlendMode = static_cast<EBlendMode>(blendModeIndex - 1); break;
			}
		}

		i32 depthModeIndex = 0;

		if (aInput.DepthMode)
			depthModeIndex = static_cast<i32>(*aInput.DepthMode) + 1;

		if (ImGui::Combo("Depth Mode", &depthModeIndex, "Current\0Greater\0GreaterEqual\0Equal\0NotEqual\0LessEqual\0Less\0Never\0Always\0"))
		{
			switch (depthModeIndex)
			{
			case 0: aInput.DepthMode.reset(); break;
			default: aInput.DepthMode = static_cast<EDepthMode>(depthModeIndex - 1); break;
			}
		}
		ImGui::PopID();

		return (aInput != initial) ? EAlterStatus::Changed : EAlterStatus::Same;
	}
};