#pragma once
#include "System/System.h"
#include "Object.h"
#include "Message/MessageTypes.h"
#include <type_traits>
#include <unordered_map>
#include <functional>

class _BaseFunctionality;

class ObjectImpulseRouter
	: public System
{
public:
	ObjectImpulseRouter();

	template<typename TMessage, typename TFunction>
	void Subscribe(Object& aObject, _BaseFunctionality& aFunctionality, TFunction aCallback);

	void Unsubscribe(const size_t aTypeHash, Object& aObject, _BaseFunctionality& aFunctionality);

private:
	template<typename TMessage>
	void HandleObjectMessage(const TMessage& aMessage);

	struct SImpulseSubscription 
	{ 
		SImpulseSubscription(_BaseFunctionality& aFunctionality) noexcept
			: myFunctionalityIdentifier(aFunctionality)
		{
		}

		std::function<void(const SObjectImpulse&)> myCallback;
		std::reference_wrapper<_BaseFunctionality> myFunctionalityIdentifier;
	};

	std::unordered_map<size_t, std::unordered_map<Object*, std::vector<SImpulseSubscription>>> mySubscriptions;

	MessageSubscriber mySubscriber;
};

template<typename TMessage, typename TFunction>
void ObjectImpulseRouter::Subscribe(Object& aObject, _BaseFunctionality& aFunctionality, TFunction aCallback)
{
	static_assert(std::is_base_of_v<SObjectImpulse, TMessage>, "Object message type must have base of SObjectImpulse!");

	SImpulseSubscription subscription(aFunctionality);

	subscription.myCallback = [aCallback] (const SObjectImpulse& aImpulse) 
	{
		aCallback(static_cast<const TMessage&>(aImpulse));
	};

	auto it = mySubscriptions.find(TMessage::GetTypeHash());

	if (it == mySubscriptions.cend())
		mySubscriber.AddRoute(BindHelper(&ObjectImpulseRouter::HandleObjectMessage<TMessage>, this));

	mySubscriptions[TMessage::GetTypeHash()][&aObject].push_back(subscription);
}

template<typename TMessage>
void ObjectImpulseRouter::HandleObjectMessage(const TMessage& aMessage)
{
	const SObjectImpulse& impulse = static_cast<const SObjectImpulse&>(aMessage);
	
	auto iterator = mySubscriptions.find(TMessage::GetTypeHash());
	
	if (iterator == mySubscriptions.cend())
		return; 

	std::vector<SImpulseSubscription>& vector = iterator->second[impulse.SelfObject];
	std::for_each(vector.rbegin(), vector.rend(), [&aMessage](const auto & aSubscription) {
		aSubscription.myCallback(aMessage);
		});
}
