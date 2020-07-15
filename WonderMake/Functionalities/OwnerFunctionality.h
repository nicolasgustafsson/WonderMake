#pragma once

#include "Functionality.h"

class OwnerFunctionality
	: public Functionality<OwnerFunctionality>
{
public:
	OwnerFunctionality(Object& aOwner);

	Object& GetOwner() const;

	virtual void OnOwnerMoved(Object& aNewAddress) noexcept override;

private:
	Object* myOwner = nullptr;
};

