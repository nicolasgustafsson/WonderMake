#pragma once

#include "System.h"

#include <string>
#include <vector>
#include <unordered_map>

class CmdLineArgsSystem
	: public System<
		Policy::Set<>,
		SystemTraits::Set<
			STSingleton>>
{
public:
	CmdLineArgsSystem(std::vector<std::wstring> aWords);

	bool HasCommand(const std::wstring& aCommand) const noexcept;
	bool HasValues(const std::wstring& aCommand) const noexcept;

	std::wstring GetFirstCommandValue(const std::wstring& aCommand) const noexcept;
	std::vector<std::wstring> GetCommandValues(const std::wstring& aCommand) const noexcept;

private:
	using CommandMap = std::unordered_map<std::wstring, std::vector<std::wstring>>;

	CommandMap ParseWords(std::vector<std::wstring> aWords);

	CommandMap myCommandList;

};
