#include "pch.h"
#include "Object.h"

#include "Functionalities/Functionality.h"

Object::~Object() noexcept
{
	Destroy();
}

void Object::Destroy() noexcept
{
	while (!myFunctionalities.empty())
	{
		myFunctionalities[myFunctionalities.size() - 1].second.Reference->Destroy(*this);
		myFunctionalities.erase(myFunctionalities.end() - 1);
	}
}

Object::Object(Object&& aOther)
{
	myFunctionalities = std::move(aOther.myFunctionalities);
	myComponents = std::move(aOther.myComponents);
}

Object& Object::operator=(Object&& aOther)
{
	Destroy();
	myFunctionalities = std::move(aOther.myFunctionalities);
	myComponents = std::move(aOther.myComponents);

	return *this;
}
