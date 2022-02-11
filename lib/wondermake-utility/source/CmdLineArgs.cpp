#include "wondermake-utility/CmdLineArgs.h"

#include "wondermake-utility/StringUtility.h"

#include <algorithm>
#include <iterator>

CmdLineArgs::CmdLineArgs(std::vector<std::string> aWords)
{
	myCommandList = ParseWords(std::move(aWords));
}

CmdLineArgs::CmdLineArgs(const std::vector<std::wstring>& aWords)
{
	std::vector<std::string> words;

	words.reserve(aWords.size());

	std::transform(aWords.cbegin(), aWords.cend(), std::back_inserter(words), [](auto& aWideWord)
		{
			return Utility::Utf8Encode(aWideWord);
		});

	myCommandList = ParseWords(std::move(words));
}

bool CmdLineArgs::HasCommand(const std::string& aCommand) const noexcept
{
	return myCommandList.find(aCommand) != myCommandList.cend();
}

bool CmdLineArgs::HasValues(const std::string& aCommand) const noexcept
{
	const auto it = myCommandList.find(aCommand);

	if (it == myCommandList.cend())
		return false;

	return !it->second.empty();
}

std::string CmdLineArgs::GetFirstCommandValue(const std::string& aCommand) const noexcept
{
	const auto it = myCommandList.find(aCommand);

	if (it == myCommandList.cend()
		|| it->second.empty())
		return "";

	return it->second.front();
}

std::vector<std::string> CmdLineArgs::GetCommandValues(const std::string& aCommand) const noexcept
{
	const auto it = myCommandList.find(aCommand);

	if (it == myCommandList.cend())
		return {};

	return it->second;
}

CmdLineArgs::CommandMap CmdLineArgs::ParseWords(std::vector<std::string> aWords)
{
	CmdLineArgs::CommandMap commands;

	auto it = commands.end();

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
