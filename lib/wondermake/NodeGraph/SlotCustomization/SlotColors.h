#pragma once

#include "Constants.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-utility/Color.h"

namespace SlotColors
{
	template<typename TSlotType>
	ImColor GetColor()
	{
		if constexpr (!Constants::IsDebugging)
			return ImColor();
		else
		{
			auto& imguiStyle = ImGui::GetStyle();
			return imguiStyle.Colors[ImGuiCol_PlotLines];
		}
	}

	template<>
	inline ImColor GetColor<float>()
	{
		return ImColor(128, 255 - 32, 128, 255);
	}

	template<>
	inline ImColor GetColor<std::string>()
	{
		return ImColor(255 - 32, 128, 255 - 32, 255);
	}

	template<>
	inline ImColor GetColor<SColor>()
	{
		return ImColor(255, 255, 255, 255);
	}
};

