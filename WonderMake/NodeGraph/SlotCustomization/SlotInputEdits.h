#pragma once
#include "Imgui/FileSelector.h"

namespace SlotInputEdits
{
	template<typename TSlotType>
	void EditInputSlot(TSlotType&)
	{
	
	}

	template<>
	inline void EditInputSlot<float>(float& aInput)
	{
		ImGui::InputFloat("", &aInput);
	}

	template<>
	inline void EditInputSlot<std::string>(std::string& aInput)
	{
		ImGui::InputText("", &aInput);
	}

	template<>
	inline void EditInputSlot<bool>(bool& aInput)
	{
		ImGui::Checkbox("", &aInput);
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
};
