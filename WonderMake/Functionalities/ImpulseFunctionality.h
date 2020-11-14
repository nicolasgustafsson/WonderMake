#pragma once
#include "Functionalities/Functionality.h"
#include "Object/ObjectImpulseRouter.h"
#include "Functionalities/OwnerFunctionality.h"

struct SImpulseKey
{
	size_t TypeHash;
	_BaseFunctionality* OwningFunctionality;
};

struct SImpulseListComponent
	: public SComponent
{
	std::vector<SImpulseKey> ImpulseKeys;
};

class ImpulseFunctionality
	: public Functionality<
		ImpulseFunctionality,
		Policy::Set<
			PAdd<ObjectImpulseRouter, PWrite>,
			PAdd<OwnerFunctionality, PRead>,
			PAdd<SImpulseListComponent, PWrite>>>
{
public:
	ImpulseFunctionality(Object& aOwner, Dependencies&& aDependencies);
	~ImpulseFunctionality();

	template <typename TMessage, typename TFunction>
	void Subscribe(_BaseFunctionality& aSubscriber, TFunction aCallback);
	
	template <typename TMessage>
	void Unsubscribe(_BaseFunctionality& aSubscriber);

	void Unsubscribe(_BaseFunctionality& aSubscriber, const size_t aTypeHash);

	void UnsubscribeAll(_BaseFunctionality& aSubscriber);
};

template <typename TMessage, typename TFunction>
void ImpulseFunctionality::Subscribe(_BaseFunctionality& aSubscriber, TFunction aCallback)
{
	Get<ObjectImpulseRouter>().Subscribe<TMessage>(Get<OwnerFunctionality>().GetOwner(), aSubscriber, aCallback);

	auto& impulseList = Get<SImpulseListComponent>();

	SImpulseKey impulseKey;
	impulseKey.TypeHash = TMessage::GetTypeHash();
	impulseKey.OwningFunctionality = &aSubscriber;

	impulseList.ImpulseKeys.push_back(impulseKey);
}

template <typename TMessage>
void ImpulseFunctionality::Unsubscribe(_BaseFunctionality& aSubscriber)
{
	Unsubscribe(aSubscriber, TMessage::GetTypeHash());
}
