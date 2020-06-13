#pragma once
class Object;

class _BaseFunctionality
	: public NonCopyable
{
public:
	_BaseFunctionality();

	virtual ~_BaseFunctionality() = default;

	virtual void OnOwnerMoved([[maybe_unused]] Object& aObject) noexcept {};

	virtual void Destroy([[maybe_unused]]Object& aObject) = 0;

	void Tick() noexcept {}
};