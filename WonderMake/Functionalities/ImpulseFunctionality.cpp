#include "pch.h"
#include "ImpulseFunctionality.h"

ImpulseFunctionality::ImpulseFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

ImpulseFunctionality::~ImpulseFunctionality()
{
	Object& Owner = Get<OwnerFunctionality>().GetOwner();
	auto& impulseList = Get<SImpulseListComponent>();

	for (i32 i = 0; i < impulseList.myImpulseKeys.size(); i++)
	{
		SImpulseKey& impulseKey = impulseList.myImpulseKeys[i];

		myRouter->Unsubscribe(impulseKey.TypeHash, Owner, *impulseKey.OwningFunctionality);
	}
}

void ImpulseFunctionality::Unsubscribe(BaseFunctionality& aSubscriber, const size_t aTypeHash)
{
	myRouter->Unsubscribe(aTypeHash, Get<OwnerFunctionality>().GetOwner(), aSubscriber);

	auto& impulseComponent = Get<SImpulseListComponent>();

	for (i32 i = 0; i < impulseComponent.myImpulseKeys.size(); i++)
	{
		SImpulseKey& impulseKey = impulseComponent.myImpulseKeys[i];

		if (impulseKey.OwningFunctionality == &aSubscriber && impulseKey.TypeHash == aTypeHash)
		{
			impulseComponent.myImpulseKeys.erase(impulseComponent.myImpulseKeys.begin() + i);
			return;
		}
	}
}

void ImpulseFunctionality::UnsubscribeAll(BaseFunctionality& aSubscriber)
{
	auto& impulseList = Get<SImpulseListComponent>();
	Object& Owner = Get<OwnerFunctionality>().GetOwner();

	for (i32 i = 0; i < impulseList.myImpulseKeys.size(); i++)
	{
		SImpulseKey& impulseKey = impulseList.myImpulseKeys[i];

		if (impulseKey.OwningFunctionality == &aSubscriber)
		{
			myRouter->Unsubscribe(impulseKey.TypeHash, Owner, aSubscriber);

			impulseList.myImpulseKeys.erase(impulseList.myImpulseKeys.begin() + i);
			i--;
		}
	}
}
