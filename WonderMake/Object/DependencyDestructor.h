#pragma once

#include "WonderMakeBase/RestrictTypes.h"
#include "WonderMakeBase/UniqueFunction.h"

#include <functional>

class Object;
struct SComponent;
class _BaseFunctionality;

class DependencyDestructor
{
public:
	DependencyDestructor(std::function<void(Object& , void*)> aDestroyFunc) noexcept;

	void Destroy(Object& aObject, SComponent& aComponent);
	void Destroy(Object& aObject, _BaseFunctionality& aFunctionality);
private:
	std::function<void(Object&, void*)> myDestroyFunc;
};
