#include "pch.h"

#include "ObjectImpulseRouter.h"

ObjectImpulseRouter::ObjectImpulseRouter()
	:mySubscriber(ERoutineId::Logic)
{

}

void ObjectImpulseRouter::Unsubscribe(const size_t aTypeHash, Object& aObject, BaseFunctionality& aFunctionality)
{
	auto subscriptionIterator = mySubscriptions.find(aTypeHash);

	if (subscriptionIterator == mySubscriptions.cend())
		return;

	auto range = subscriptionIterator->second.equal_range(&aObject);

	for (auto it = range.first; it != range.second; it++)
	{
		if (&it->second.myFunctionalityIdentifier.get() == &aFunctionality)
		{
			subscriptionIterator->second.erase(it);
			return;
		}
	}
}
