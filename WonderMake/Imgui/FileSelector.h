#pragma once
#include <filesystem>

namespace ImGui::FileSelector
{
    namespace Implementation
    {
        void FinishSelecting();
    }
	bool SelectFile(std::filesystem::path& aPath);
};

