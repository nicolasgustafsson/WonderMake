#pragma once
#include <json/json.hpp>
class Id
{
public:
	Id() {};
	Id(u64 aId) : myId(aId) {}

	[[nodiscard]] bool operator==(Id aOther) const { return aOther.myId == myId; }
	
	[[nodiscard]] u64 GetRawId() const noexcept { return myId; }

private:
	u64 myId = 0;
};

class IdCounter
{
public:
	Id NextId();
private:
	u64 myInternalId = 0;
};

//Unique id per object per class
template<typename T>
class UniqueId : public Id
{
public:
	UniqueId() : Id(UniqueIdCounter.NextId()) {}

private:
	static IdCounter UniqueIdCounter;
};


namespace std {
	template <> struct hash<Id>
	{
		size_t operator()(const Id& aId) const
		{
			return static_cast<size_t>(aId.GetRawId());
		}
	};
}

inline void to_json(json& aJson, const Id& aId)
{
	aJson = { {"Id", aId.GetRawId()} };
}

inline void from_json(const json& aJson, Id& aId)
{
	aId = Id(aJson["Id"].get<u64>());
}