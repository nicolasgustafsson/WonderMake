#include "pch.h"

#include "FileSelector.h"

#include "wondermake-debug-ui/ImguiInclude.h"

#include "wondermake-utility/WinPlatform.h"

#include <filesystem>

bool ImGui::FileSelector::SelectFile(std::filesystem::path& aPath)
{
	if(aPath.empty())
		ImGui::Text("Please select a file");
	else
		ImGui::Text(aPath.string().c_str());

	if (ImGui::Button("Select File"))
	{
		char filename[MAX_PATH];

		std::filesystem::path originalPath = std::filesystem::current_path();
		std::string currentPath = std::filesystem::current_path().string();

		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrInitialDir = currentPath.c_str();
		ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "Select a File";
		ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameA(&ofn))
		{
			aPath = filename;
			aPath = std::filesystem::relative(aPath, originalPath);
		}

		//[Nicos]: for some reason using the open file dialog changes the current path, so reset it here.
		std::filesystem::current_path(originalPath);

		return true;
	}

	return false;
}
