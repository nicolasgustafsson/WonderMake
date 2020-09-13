#include "pch.h"
#include "TargetFunctionality.h"

REGISTER_FUNCTIONALITY(TargetFunctionality);

TargetFunctionality::TargetFunctionality(Object& aOwner, Dependencies&& aDependencies)
	: Super(aOwner, std::move(aDependencies))
{

}
