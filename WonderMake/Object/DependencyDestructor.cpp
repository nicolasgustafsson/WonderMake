#include "pch.h"
#include "DependencyDestructor.h"

DependencyDestructor::DependencyDestructor(UniqueFunction<void(BaseObject&, void*)> aDestroyFunc) noexcept
	: myDestroyFunc(std::move(aDestroyFunc))
{}

void DependencyDestructor::Destroy(BaseObject& aObject, SComponent& aComponent)
{
	myDestroyFunc(aObject, &aComponent);
}

void DependencyDestructor::Destroy(BaseObject& aObject, _BaseFunctionality& aFunctionality)
{
	myDestroyFunc(aObject, &aFunctionality);
}
