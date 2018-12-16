#include "stdafx.h"
#include "MessageSubscriber.h"

#include "DataRouters.h"
#include "DispatchRouter.h"

MessageSubscriber::~MessageSubscriber()
{
	for (const size_t typeHash : mySubscribedMessages)
	{
		DataRouters::Get().GetRouter(myThreadId).UnsubscribeToType(typeHash, *this);
	}
}

void MessageSubscriber::Subscribe(const size_t aTypeHash, std::function<void(const Dispatchable&)>&& aCallback)
{
	mySubscribedMessages.insert(aTypeHash);
	DataRouters::Get().GetRouter(myThreadId).SubscribeToType(aTypeHash, *this, std::move(aCallback));
}
