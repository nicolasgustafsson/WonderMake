#pragma once

class SystemContainer;

class System : public NonCopyable
{
public:
	virtual void Tick() {}

protected:
	System() = default;
	virtual ~System() = default;
};

