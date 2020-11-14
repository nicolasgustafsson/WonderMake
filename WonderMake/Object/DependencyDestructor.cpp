#include "pch.h"
#include "DependencyDestructor.h"

DependencyDestructor::DependencyDestructor(std::function<void(Object&, void*)>&& aDestroyFunc) noexcept
	: myDestroyFunc(std::move(aDestroyFunc))
{}

void DependencyDestructor::Destroy(Object& aObject, SComponent& aComponent)
{
	myDestroyFunc(aObject, &aComponent);
}

void DependencyDestructor::Destroy(Object& aObject, _BaseFunctionality& aFunctionality)
{
	myDestroyFunc(aObject, &aFunctionality);
}
