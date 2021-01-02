#pragma once
class Id
{
public:
	Id();
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


namespace std {
	template <> struct hash<Id>
	{
		size_t operator()(const Id& aId) const
		{
			return aId.GetRawId();
		}
	};
}