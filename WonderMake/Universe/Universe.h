#pragma once

class Universe
{
public:
	[[nodiscard]] bool operator==(const Universe& aOther) const { return aOther.myId == myId; }
	UniqueId<Universe> myId;
};
