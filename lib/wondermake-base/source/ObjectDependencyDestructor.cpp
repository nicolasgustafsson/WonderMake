#include "wondermake-base/ObjectDependencyDestructor.h"

ObjectDependencyDestructor::ObjectDependencyDestructor(std::function<void(Object&, void*)> aDestroyFunc) noexcept
	: myDestroyFunc(std::move(aDestroyFunc))
{}

void ObjectDependencyDestructor::Destroy(Object& aObject, SComponent& aComponent)
{
	myDestroyFunc(aObject, &aComponent);
}

void ObjectDependencyDestructor::Destroy(Object& aObject, _BaseFunctionality& aFunctionality)
{
	myDestroyFunc(aObject, &aFunctionality);
}
