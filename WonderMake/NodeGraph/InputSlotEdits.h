#pragma once
namespace InputSlotEdits
{
	template<typename TSlotType>
	void EditInputSlot(TSlotType& aInput)
	{
		//auto& imguiStyle = ImGui::GetStyle();
		//return imguiStyle.Colors[ImGuiCol_PlotLines];
	}

	template<>
	inline void EditInputSlot<float>(float& aInput)
	{
		ImGui::InputFloat("", &aInput);
		//return ImColor(128, 255 - 32, 128, 255);
	}

	template<>
	inline void EditInputSlot<std::string>(std::string& aInput)
	{
		ImGui::InputText("", &aInput);

		//return ImColor(255 - 32, 128, 255 - 32, 255);
	}

	template<>
	inline void EditInputSlot<SColor>(SColor& aInput)
	{
		ImGui::ColorEdit4("", ((f32*)&aInput));

		//return ImColor(255 - 32, 128, 255 - 32, 255);
	}
};

