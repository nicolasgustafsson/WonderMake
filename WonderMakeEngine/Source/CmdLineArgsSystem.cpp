#include "WonderMakeEngine/CmdLineArgsSystem.h"

CmdLineArgsSystem::CmdLineArgsSystem(std::vector<std::wstring> aWords)
{
	myCommandList = ParseWords(std::move(aWords));
}

CmdLineArgsSystem::CommandMap CmdLineArgsSystem::ParseWords(std::vector<std::wstring> aWords)
{
	CmdLineArgsSystem::CommandMap commands;

	CmdLineArgsSystem::CommandMap::iterator it = commands.end();

	for (auto&& word : aWords)
	{
		if (word.starts_with('-'))
		{
			if (word.size() == 1)
				continue;

			word.erase(word.begin());

			const auto insertPair = commands.insert({ std::move(word) , {} });

			it = insertPair.first;
		}
		else if (it != commands.cend())
		{
			it->second.emplace_back(std::move(word));
		}
	}

	return commands;
}

bool CmdLineArgsSystem::HasCommand(const std::wstring& aCommand) const noexcept
{
	return myCommandList.find(aCommand) != myCommandList.cend();
}

bool CmdLineArgsSystem::HasValues(const std::wstring& aCommand) const noexcept
{
	const auto it = myCommandList.find(aCommand);

	if (it == myCommandList.cend())
		return false;

	return !it->second.empty();
}

std::wstring CmdLineArgsSystem::GetFirstCommandValue(const std::wstring& aCommand) const noexcept
{
	const auto it = myCommandList.find(aCommand);

	if (it == myCommandList.cend()
		|| it->second.empty())
		return L"";

	return it->second.front();
}

std::vector<std::wstring> CmdLineArgsSystem::GetCommandValues(const std::wstring& aCommand) const noexcept
{
	const auto it = myCommandList.find(aCommand);

	if (it == myCommandList.cend())
		return {};

	return it->second;
}
