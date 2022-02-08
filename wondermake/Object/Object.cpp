#include "pch.h"
#include "Object.h"

#include "Functionalities/BaseFunctionality.h"

Object::~Object() noexcept
{
	Destroy();
}

void Object::Destroy()
{
	while (!myFunctionalities.empty())
	{
		const auto it = std::find_if(myFunctionalities.begin(), myFunctionalities.end(), [](const auto& aFuncPair)
			{
				const auto& counter = aFuncPair.second;

				return counter.RefCount == 0;
			});

		assert(it != myFunctionalities.cend() && "blo");

		const auto& [index, counter] = *it;

		auto& ref = *counter.Reference;
		auto& destructor = *counter.Destructor;

		myFunctionalities.erase(it);

		destructor.Destroy(*this, ref);
	}

	for (const auto&[index, counter] : myComponents)
	{
		counter.Destructor->Destroy(*this, *counter.Reference);
	}
	
	myComponents.clear();
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
