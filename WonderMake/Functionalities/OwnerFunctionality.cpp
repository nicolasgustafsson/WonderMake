#include "pch.h"
#include "OwnerFunctionality.h"

OwnerFunctionality::OwnerFunctionality(Object& aOwner)
	: Super(aOwner)
	, myOwner(aOwner)
{

}

Object& OwnerFunctionality::GetOwner() const
{
	return myOwner;
}
