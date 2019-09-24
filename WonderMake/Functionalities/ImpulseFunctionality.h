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
	: public Functionality<ImpulseFunctionality, OwnerFunctionality, SImpulseListComponent>
{
public:
	ImpulseFunctionality(Object& aOwner);
	~ImpulseFunctionality();

	template <typename TMessage, typename TFunction>
	void Subscribe(_BaseFunctionality& aSubscriber, TFunction aCallback);
	
	template <typename TMessage>
	void Unsubscribe(_BaseFunctionality& aSubscriber);

	void Unsubscribe(_BaseFunctionality& aSubscriber, const size_t aTypeHash);

	void UnsubscribeAll(_BaseFunctionality& aSubscriber);

private:
	SystemPtr<ObjectImpulseRouter> myRouter;
};

template <typename TMessage, typename TFunction>
void ImpulseFunctionality::Subscribe(_BaseFunctionality& aSubscriber, TFunction aCallback)
{
	myRouter->Subscribe<TMessage>(Get<OwnerFunctionality>().GetOwner(), aSubscriber, aCallback);

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
