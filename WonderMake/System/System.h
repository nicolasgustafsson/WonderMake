#pragma once

class SystemContainer;

class System : public NonCopyable
{
public:
	virtual void Tick() {}

protected:
	void EnableTick();
	System() = default;
	virtual ~System() = default;
};

