#include "pch.h"
#include "MessageSubscriber.h"

#include "DispatchRouter.h"

MessageSubscriber::~MessageSubscriber()
{
	for (const size_t typeHash : mySubscribedMessages)
	{
		DispatchRouter::Get().UnsubscribeToType(typeHash, *this);
	}
}

void MessageSubscriber::Subscribe(const size_t aTypeHash, UniqueFunction<void(const Dispatchable&)> aCallback)
{
	mySubscribedMessages.insert(aTypeHash);
	DispatchRouter::Get().SubscribeToType(aTypeHash, *this, std::move(aCallback));
}

void MessageSubscriber::RemoveRoute(const size_t aTypeHash)
{	
	mySubscribedMessages.erase(aTypeHash);
	DispatchRouter::Get().UnsubscribeToType(aTypeHash, *this);
}
