#include "pch.h"

#include "ObjectImpulseRouter.h"

REGISTER_SYSTEM(ObjectImpulseRouter);

ObjectImpulseRouter::ObjectImpulseRouter(Dependencies&& aDependencies)
	: Super(std::move(aDependencies))
	, mySubscriber(ERoutineId::Logic)
{

}

void ObjectImpulseRouter::Unsubscribe(const size_t aTypeHash, Object& aObject, _BaseFunctionality& aFunctionality)
{
	auto subscriptionIterator = mySubscriptions.find(aTypeHash);

	if (subscriptionIterator == mySubscriptions.cend())
		return;

	auto& vector = subscriptionIterator->second[&aObject];
	 
	auto it = std::find_if(vector.begin(), vector.end(), [&aFunctionality](const auto& aSubscription)
		{
			return (&aSubscription.myFunctionalityIdentifier.get() == &aFunctionality);
		});

	if (it == vector.cend())
	{
		return;
	}

	vector.erase(it);
}

