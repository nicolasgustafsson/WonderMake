#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class CmdLineArgs
{
public:
	CmdLineArgs(std::vector<std::string> aWords);
	CmdLineArgs(const std::vector<std::wstring>& aWords);

	bool HasCommand(const std::string& aCommand) const noexcept;
	bool HasValues(const std::string& aCommand) const noexcept;

	std::string GetFirstCommandValue(const std::string& aCommand) const noexcept;
	std::vector<std::string> GetCommandValues(const std::string& aCommand) const noexcept;

private:
	using CommandMap = std::unordered_map<std::string, std::vector<std::string>>;

	static CommandMap ParseWords(std::vector<std::string> aWords);

	CommandMap myCommandList;

};
