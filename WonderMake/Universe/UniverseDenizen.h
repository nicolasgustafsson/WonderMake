#pragma once
#include "UniverseManagerSystem.h"

class SUniverseDenizenComponent : public SComponent
{
public:

private:
	friend class UniverseDenizen;
	Id myUniverseId;
};

class UniverseDenizen : public Functionality<SUniverseDenizenComponent, UniverseManagerSystem>
{
public:
	void SetUniverse(std::string aUniverseName);
	Id GetUniverseId() const;
private:
};

