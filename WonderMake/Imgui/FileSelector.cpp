#include "pch.h"
#include "FileSelector.h"
#include <filesystem>

#if COMPILER_MSVC
#include <commdlg.h>
#endif

#if COMPILER_GCC
static std::filesystem::path CurrentPath = std::filesystem::current_path();
#endif
bool ImGui::FileSelector::SelectFile(std::filesystem::path &aPath) {
    if (aPath.empty())
        ImGui::Text("Please select a file");
    else
        ImGui::Text(aPath.string().c_str());

#if COMPILER_MSVC
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
#endif

#if COMPILER_GCC
    static std::optional<i32> selectedIndex = std::nullopt;
    if (ImGui::Button("Select File")) {
        ImGui::OpenPopup("Select File");
        selectedIndex = std::nullopt;
    }

    ImGui::SetNextWindowSize({400.f, 415.f});
    if (ImGui::BeginPopupModal("Select File", nullptr, ImGuiWindowFlags_NoResize)) {
        ImGui::BeginChild("File List", {ImGui::GetContentRegionAvail().x, 350.f});
        i32 itemIndex = 0;
        std::optional<std::filesystem::path> selectedPath;

        if (CurrentPath != std::filesystem::current_path())
        {
            if (ImGui::Selectable("..", selectedIndex && selectedIndex == itemIndex, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick))
            {
                selectedIndex = itemIndex;

                if (ImGui::IsMouseDoubleClicked(0))
                {
                    CurrentPath = CurrentPath.parent_path();

                    selectedIndex = std::nullopt;
                }
            }
        }
        for (auto const &entry: std::filesystem::directory_iterator{CurrentPath})
        {
            itemIndex++;
            if (ImGui::Selectable(entry.path().string().c_str(), selectedIndex && selectedIndex == itemIndex, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick))
            {
                selectedIndex = itemIndex;
                selectedPath = entry.path();
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    if (entry.is_regular_file())
                    {
                        if (selectedPath)
                            aPath = *selectedPath;
                        ImGui::EndChild();
                        Implementation::FinishSelecting();

                        CurrentPath = std::filesystem::current_path();
                        return true;
                    }
                    else if (entry.is_directory())
                    {
                        CurrentPath = entry.path();
                        selectedIndex = std::nullopt;
                        break;
                    }
                }
            }
        }
        ImGui::EndChild();
        ImGui::Separator();

        ImGui::Spacing();
        ImGui::SameLine(ImGui::GetWindowSize().x - 120.f);

        ImGui::BeginDisabled(selectedPath.has_value());
        if (ImGui::Button("Select"))
        {
            if (selectedPath)
                aPath = *selectedPath;
            Implementation::FinishSelecting();
            return true;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            Implementation::FinishSelecting();
            return false;
        }

        ImGui::EndPopup();
    }
#endif
    return false;
}

void ImGui::FileSelector::Implementation::FinishSelecting()
{
    ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
    CurrentPath = std::filesystem::current_path();
}
