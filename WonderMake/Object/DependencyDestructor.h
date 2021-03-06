#pragma once

#include "Utilities/RestrictTypes.h"
#include "Utilities/UniqueFunction.h"

class Object;
struct SComponent;
class _BaseFunctionality;

class DependencyDestructor
{
public:
	DependencyDestructor(UniqueFunction<void(Object& , void*)> aDestroyFunc) noexcept;

	void Destroy(Object& aObject, SComponent& aComponent);
	void Destroy(Object& aObject, _BaseFunctionality& aFunctionality);
private:
	UniqueFunction<void(Object&, void*)> myDestroyFunc;
};
