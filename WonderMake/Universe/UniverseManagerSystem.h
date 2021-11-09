#pragma once
#include "System.h"
#include "Utilities/Id.h"

class UniverseManagerSystem : public System<>
{
public:
	UniverseManagerSystem()
	{
		myCurrentUniverseName = "Game";
		myUniverseStack.Add(GetUniverseId(std::move("Game")));
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

	[[nodiscard]] struct UniverseStackGuard PushUniverse(const std::string aUniverseName);

private:
	void PopUniverse()
	{
		myUniverseStack.EraseAt(myUniverseStack.Count() - 1);
		myCurrentUniverseName = GetUniverseName(myUniverseStack[myUniverseStack.Count() - 1]);
	}

	Container<Id> myUniverseStack;

	std::string myCurrentUniverseName = "";

	friend struct UniverseStackGuard;

	mutable Container<std::string, Key<u64>> myUniverseIdsToName;
};

struct UniverseStackGuard : public NonCopyable, public NonMovable
{
	~UniverseStackGuard();

private:
	UniverseStackGuard() = default;
	friend class UniverseManagerSystem;
};
