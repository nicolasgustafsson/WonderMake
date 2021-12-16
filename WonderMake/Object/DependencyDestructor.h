#pragma once

#include "Utilities/RestrictTypes.h"
#include "Utilities/UniqueFunction.h"

class BaseObject;
struct SComponent;
class _BaseFunctionality;

class DependencyDestructor
{
public:
	DependencyDestructor(UniqueFunction<void(BaseObject& , void*)> aDestroyFunc) noexcept;

	void Destroy(BaseObject& aObject, SComponent& aComponent);
	void Destroy(BaseObject& aObject, _BaseFunctionality& aFunctionality);
private:
	UniqueFunction<void(BaseObject&, void*)> myDestroyFunc;
};
