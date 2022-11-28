#pragma once

#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/UniqueFunction.h"

#include <functional>

class Object;
struct SComponent;
class _BaseFunctionality;

class ObjectDependencyDestructor
{
public:
	ObjectDependencyDestructor(std::function<void(Object& , void*)> aDestroyFunc) noexcept;

	void Destroy(Object& aObject, SComponent& aComponent);
	void Destroy(Object& aObject, _BaseFunctionality& aFunctionality);
private:
	std::function<void(Object&, void*)> myDestroyFunc;
};
