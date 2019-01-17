#pragma once

class SystemContainer;

class System
{
public:
	virtual void Update() {}

protected:
	System() = default;
	friend class SystemContainer;
};

