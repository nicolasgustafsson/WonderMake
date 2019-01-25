#pragma once

class BaseFunctionality
{
public:
	virtual ~BaseFunctionality() = default;

	virtual void Tick() {}
};

//describes a functionality for an object
class Functionality
	:public BaseFunctionality
{
public:
	Functionality();
};

