#pragma once
#include "Functionalities/Functionality.h"
#include "Object/ObjectImpulseRouter.h"
#include "Functionalities/OwnerFunctionality.h"

struct SImpulseKey
{
	size_t TypeHash;
	BaseFunctionality* OwningFunctionality;
};

struct SImpulseListComponent
	: public SComponent
{
	std::vector<SImpulseKey> myImpulseKeys;
};

class ImpulseFunctionality
	: public Functionality<ImpulseFunctionality, OwnerFunctionality, SImpulseListComponent>
{
public:
	ImpulseFunctionality(Object& aOwner);
	~ImpulseFunctionality();

	template <typename TMessage>
	void Subscribe(BaseFunctionality& aSubscriber, std::function<void(const TMessage&)> aCallback);
	
	template <typename TMessage>
	void Unsubscribe(BaseFunctionality& aSubscriber);

	void Unsubscribe(BaseFunctionality& aSubscriber, const size_t aTypeHash);

	void UnsubscribeAll(BaseFunctionality& aSubscriber);

private:
	SystemPtr<ObjectImpulseRouter> myRouter;
};

template <typename TMessage>
void ImpulseFunctionality::Subscribe(BaseFunctionality& aSubscriber, std::function<void(const TMessage&)> aCallback)
{
	myRouter->Subscribe(Get<OwnerFunctionality>().GetOwner(), aSubscriber, aCallback);

	auto& impulseList = Get<SImpulseListComponent>();

	SImpulseKey impulseKey;
	impulseKey.TypeHash = TMessage::GetTypeHash();
	impulseKey.OwningFunctionality = &aSubscriber;

	impulseList.myImpulseKeys.push_back(impulseKey);
}

template <typename TMessage>
void ImpulseFunctionality::Unsubscribe(BaseFunctionality& aSubscriber)
{
	Unsubscribe(aSubscriber, TMessage::GetTypeHash());
}
