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
