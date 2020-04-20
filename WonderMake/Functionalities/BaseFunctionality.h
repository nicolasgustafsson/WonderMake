#pragma once

class Object;

class _BaseFunctionality
	: public NonCopyable
{
public:
	_BaseFunctionality() = default;

	virtual ~_BaseFunctionality() = default;

	virtual void Destroy([[maybe_unused]] Object& aObject) = 0;

	void Tick() noexcept {}
};