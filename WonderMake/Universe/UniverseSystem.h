#pragma once

class UniverseSystem : public System<>
{
public:
	[[nodiscard]] Id GetUniverseId(std::string aUniverseName)
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

	Container<std::string, Key<u64>> myUniverseIdsToName;
};
