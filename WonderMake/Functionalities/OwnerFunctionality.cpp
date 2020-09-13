#include "pch.h"
#include "OwnerFunctionality.h"

REGISTER_FUNCTIONALITY(OwnerFunctionality);

OwnerFunctionality::OwnerFunctionality(Object& aOwner, Dependencies&& aDependencies)
	: Super(aOwner, std::move(aDependencies))
	, myOwner(&aOwner)
{}

Object& OwnerFunctionality::GetOwner() const
{
	return *myOwner;
}

void OwnerFunctionality::OnOwnerMoved(Object& aNewAddress) noexcept
{
	myOwner = &aNewAddress;
}
