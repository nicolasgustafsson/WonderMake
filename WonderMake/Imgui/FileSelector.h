#pragma once
#include <filesystem>

namespace WmGui::FileSelector
{
    namespace Implementation
    {
        void FinishSelecting();
    }
	bool SelectFile(std::filesystem::path& aPath);
};

