#pragma once

#include "Functionality.h"

class OwnerFunctionality
	: public Functionality<>
{
public:
	OwnerFunctionality(Object& aOwner, Dependencies&& aDependencies);

	Object& GetOwner() const;

	virtual void OnOwnerMoved(Object& aNewAddress) noexcept override;

private:
	Object* myOwner = nullptr;
};
