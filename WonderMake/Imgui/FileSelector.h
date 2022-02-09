#pragma once
#include <filesystem>

namespace ImGui::FileSelector
{
	bool SelectFile(std::filesystem::path& aPath);
};

