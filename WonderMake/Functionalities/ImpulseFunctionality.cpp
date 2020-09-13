#include "pch.h"
#include "ImpulseFunctionality.h"
#include <algorithm>

REGISTER_COMPONENT(SImpulseListComponent);
REGISTER_FUNCTIONALITY(ImpulseFunctionality);

ImpulseFunctionality::ImpulseFunctionality(Object& aOwner, Dependencies&& aDependencies)
	: Super(aOwner, std::move(aDependencies))
{}

ImpulseFunctionality::~ImpulseFunctionality()
{
	Object& Owner = Get<OwnerFunctionality>().GetOwner();
	auto& impulseList = Get<SImpulseListComponent>();

	for (SImpulseKey& impulseKey : impulseList.ImpulseKeys)
	{
		Get<ObjectImpulseRouter>().Unsubscribe(impulseKey.TypeHash, Owner, *impulseKey.OwningFunctionality);
	}
}

void ImpulseFunctionality::Unsubscribe(_BaseFunctionality& aSubscriber, const size_t aTypeHash)
{
	Get<ObjectImpulseRouter>().Unsubscribe(aTypeHash, Get<OwnerFunctionality>().GetOwner(), aSubscriber);

	auto& impulseComponent = Get<SImpulseListComponent>();

	auto it = std::find_if(impulseComponent.ImpulseKeys.begin(), impulseComponent.ImpulseKeys.end(), [&aSubscriber, aTypeHash](SImpulseKey& aKey) -> bool 
		{
			return aKey.OwningFunctionality == &aSubscriber && aKey.TypeHash == aTypeHash; 
		});

	if (it != impulseComponent.ImpulseKeys.end())
		impulseComponent.ImpulseKeys.erase(it);
}

void ImpulseFunctionality::UnsubscribeAll(_BaseFunctionality& aSubscriber)
{
	auto& impulseList = Get<SImpulseListComponent>();
	Object& owner = Get<OwnerFunctionality>().GetOwner();

	const auto it = std::remove_if(impulseList.ImpulseKeys.begin(), impulseList.ImpulseKeys.end(), [&aSubscriber](const auto& aImpulseKey)
		{
			return aImpulseKey.OwningFunctionality == &aSubscriber;
		});

	std::for_each(it, impulseList.ImpulseKeys.end(), [&](const auto& aImpulseKey)
		{
			Get<ObjectImpulseRouter>().Unsubscribe(aImpulseKey.TypeHash, owner, aSubscriber);
		});

	impulseList.ImpulseKeys.erase(it, impulseList.ImpulseKeys.end());
}
