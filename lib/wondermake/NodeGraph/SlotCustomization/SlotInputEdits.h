#pragma once
#include "Imgui/FileSelector.h"

#include "wondermake-ui/RenderSettingsManager.h"

#include "wondermake-utility/Vector.h"

namespace SlotInputEdits
{
	template<typename TSlotType>
	void EditInputSlot(TSlotType&)
	{
	
	}

	template<>
	inline void EditInputSlot<float>(float& aInput)
	{
		ImGui::InputFloat("##", &aInput);
	}

	template<>
	inline void EditInputSlot<std::string>(std::string& aInput)
	{
		ImGui::InputText("##", &aInput);
	}

	template<>
	inline void EditInputSlot<bool>(bool& aInput)
	{
		ImGui::Checkbox("##", &aInput);
	}

	template<>
	inline void EditInputSlot<SVector2u>(SVector2u& aInput)
	{
		i32 inputs[]{ static_cast<i32>(aInput.X), static_cast<i32>(aInput.Y)};
		ImGui::InputInt2("", inputs);

		aInput.X = static_cast<u32>(inputs[0]);
		aInput.Y = static_cast<u32>(inputs[1]);
	}

	template<>
	inline void EditInputSlot<SColor>(SColor& aInput)
	{
		ImGui::ColorEdit4("", ((f32*)&aInput));
	}

	template<>
	inline void EditInputSlot<std::filesystem::path>(std::filesystem::path& aInput)
	{
		ImGui::FileSelector::SelectFile(aInput);
	}

	template<>
	inline void EditInputSlot<SRenderSettings>(SRenderSettings& aInput)
	{
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
	}
};