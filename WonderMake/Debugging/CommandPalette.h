#pragma once
#include "Input/InputSystem.h"

namespace Wm
{
    struct SCommand
    {
        std::string Name;
        Closure Function;
    };
    class CommandPalette : public System<InputSystem>, Debugged
    {
    public:
        CommandPalette() : Debugged("CommandPalette")
        {
            WmLog(TagSuccess, "Created the command palette!!");
        }

        void RegisterCommand(SCommand&& aCommand);

    protected:
        void Debug() override;

    private:
        Container<std::string> GatherSuggestions(const std::string& aCommand);

        const SCommand& ParseTextToCommand(const std::string& aText);
        Container<SCommand, Key<std::string>> myCommands;
        std::string myCurrentCommand{};
        Container<std::string> myCurrentSuggestions;
        std::optional<i32> myHoveredSuggestion = std::nullopt;
    };
}
