#include "pch.h"
#include "Object.h"

#include "Functionalities/Functionality.h"

Object::Object(Object&& aOther) noexcept
{
	*this = std::move(aOther);
}

Object::~Object() noexcept
{
	Destroy();
}

Object& Object::operator=(Object&& aOther) noexcept
{
	Destroy();

	myFunctionalities = std::move(aOther.myFunctionalities);
	myComponents = std::move(aOther.myComponents);

	return *this;
}

void Object::Destroy() noexcept
{
	while (!myFunctionalities.empty())
	{
		myFunctionalities[myFunctionalities.size() - 1].second.Reference->Destroy(*this);
		myFunctionalities.erase(myFunctionalities.end() - 1);
	}
}