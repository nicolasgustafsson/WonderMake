#pragma once

class BaseCapability
{
public:
	virtual ~BaseCapability() = default;

	virtual void Tick() {}
};

//describes a capability for an object
class Capability
	:public BaseCapability
{
public:
	Capability();
};

