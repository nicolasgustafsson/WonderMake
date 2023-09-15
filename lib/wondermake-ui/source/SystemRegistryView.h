#pragma once

#include "wondermake-base/System.h"
#include "wondermake-base/SystemCounter.h"

#include <string>
#include <map>
#include <optional>
#include <unordered_map>
#include <unordered_set>

class DebugSystem;

class SystemRegistryView
	: public System<
		Policy::Set<
			PAdd<DebugSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
	, public std::enable_shared_from_this<SystemRegistryView>
{
public:
	void Initialize() override;

private:
	struct SInfo
	{
		SystemId						Id;
		std::string						TypeName;
		std::unordered_set<SystemId>	Dependencies;
		std::unordered_set<SystemId>	Uses;
	};

	void Debug(bool& aIsOpen);

	void UpdateCache(const std::unordered_map<SystemId, SystemCounter::SSystemInfo>& aInfo);

	std::unordered_map<SystemId, SystemCounter::SSystemInfo>	myCache;
	std::map<u32, std::multimap<std::string, SystemId>>			mySortedIds;
	std::unordered_map<SystemId, SInfo>							myTransformedCache;

	std::optional<SystemId>	mySelectedSystem;

};
