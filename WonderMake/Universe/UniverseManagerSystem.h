#pragma once
#include "System.h"
#include "Utilities/Id.h"

class UniverseManagerSystem : public System<>
{
public:
	UniverseManagerSystem()
	{
		PushUniverse("Game");
	}

	[[nodiscard]] Id GetUniverseId(const std::string aUniverseName) const
	{
		u64 hash = std::hash<std::string>{}(aUniverseName);
		Id id(hash);
		myUniverseIdsToName[hash] = std::string(aUniverseName);

		return id;
	}

	std::string GetUniverseName(const Id aId)
	{
		if (myUniverseIdsToName.KeyExists(aId.GetRawId()))
			return myUniverseIdsToName.Get(aId.GetRawId());

		return "Unknown Universe";
	}

	Id GetCurrentUniverse() const
	{
		return myUniverseStack[myUniverseStack.Count() - 1];
	}

	void PushUniverse(const std::string aUniverseName)
	{
		myCurrentUniverseName = aUniverseName;
		myUniverseStack.Add(GetUniverseId(std::move(aUniverseName)));
	}

	void PopUniverse()
	{
		myUniverseStack.EraseAt(myUniverseStack.Count() - 1);
		myCurrentUniverseName = GetUniverseName(myUniverseStack[myUniverseStack.Count() - 1]);
	}

	Container<Id> myUniverseStack;

	std::string myCurrentUniverseName = "";

	mutable Container<std::string, Key<u64>> myUniverseIdsToName;
};
