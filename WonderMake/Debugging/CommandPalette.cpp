#include "pch.h"
#include "CommandPalette.h"

namespace Wm
{
    REGISTER_SYSTEM(CommandPalette);

    void CommandPalette::Debug()
    {
        bool shouldFocusTextbox = true;
        if (Get<InputSystem>().IsKeyPressed(EKeyboardKey::Tilde) && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
        {
            ImGui::OpenPopup("Command Palette");
            shouldFocusTextbox = true;
            myCurrentCommand.clear();
            myHoveredSuggestion = std::nullopt;
        }

        if (ImGui::BeginPopupModal("Command Palette", nullptr, ImGuiWindowFlags_AlwaysAutoResize  | ImGuiWindowFlags_NoTitleBar))
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            if (Get<InputSystem>().IsKeyPressed(EKeyboardKey::Escape))
                ImGui::CloseCurrentPopup();

            //todo only gather suggestions when text changes; reset myhoveredsuggestion when new suggestions are gathered.
            if (shouldFocusTextbox)
                ImGui::SetKeyboardFocusHere();

            std::string preInputCommand = myCurrentCommand;
            const bool inputTextPulsed = ImGui::InputText("", &myCurrentCommand, ImGuiInputTextFlags_EnterReturnsTrue);
            if (preInputCommand != myCurrentCommand)
            {
                myCurrentSuggestions = GatherSuggestions(myCurrentCommand);
                myHoveredSuggestion.reset();
            }

            if (inputTextPulsed)
            {
                if (myHoveredSuggestion && myHoveredSuggestion < myCurrentSuggestions.Count())
                {
                    myCurrentCommand = myCurrentSuggestions[*myHoveredSuggestion];
                }
                auto&& command = ParseTextToCommand(myCurrentCommand);
                command.Function();

                if (!Get<InputSystem>().IsKeyDown(EKeyboardKey::Shift))
                    ImGui::CloseCurrentPopup();
            }
            auto&& suggestions = GatherSuggestions(myCurrentCommand);
            if(Get<InputSystem>().IsKeyPressed(EKeyboardKey::Down) || !Get<InputSystem>().IsKeyPressed(EKeyboardKey::Shift) && Get<InputSystem>().IsKeyPressed(EKeyboardKey::Tab))
            {
                myHoveredSuggestion = myHoveredSuggestion.has_value() ? (*myHoveredSuggestion) + 1 : 0;
                if (myHoveredSuggestion >= myCurrentSuggestions.Count())
                    myHoveredSuggestion = 0;
            }
            else if (Get<InputSystem>().IsKeyPressed(EKeyboardKey::Up) || Get<InputSystem>().IsKeyPressed(EKeyboardKey::Shift) && Get<InputSystem>().IsKeyPressed(EKeyboardKey::Tab))
            {
                myHoveredSuggestion = myHoveredSuggestion.has_value() ? (*myHoveredSuggestion) - 1 : suggestions.Count() - 1;
                if (myHoveredSuggestion < 0)
                    myHoveredSuggestion = myCurrentSuggestions.Count() - 1;
            }

            i32 index = 0;
            for(auto&& suggestion : myCurrentSuggestions)
            {
                ImGui::Selectable(suggestion.c_str(), myHoveredSuggestion == index);
                index++;
            }

            ImGui::EndPopup();
        }
    }

    void CommandPalette::RegisterCommand(SCommand&& aCommand)
    {
        WmLog(TagSuccess, "Added command: ", aCommand.Name);
        const std::string name = aCommand.Name;
        myCommands.Add(name, std::move(aCommand));
    }

    const SCommand& CommandPalette::ParseTextToCommand(const std::string& aText)
    {
        static SCommand nullCommand{{}, [](){}};
        if (!myCommands.KeyExists(aText))
        {
            WmLog(TagError, "Could not execute command [", aText, "]. Command does not exist!");
            return nullCommand;
        }
        return myCommands.Get(aText);
    }

    Container<std::string> CommandPalette::GatherSuggestions(const std::string& aCommand)
    {
        Container<std::string> suggestions;

        for(auto&& command : myCommands)
        {
            if (command.first.starts_with(aCommand))
                suggestions.Add(command.first);
        }

        return suggestions;
    }
}
