#include "stdafx.h"
#include "MessageRouter.h"

#include "DataRouters.h"
#include "DispatchRouter.h"

MessageRouter::~MessageRouter()
{
	for (const size_t typeHash : mySubscribedMessages)
	{
		DataRouters::Get().GetRouter(myThreadId).UnsubscribeToType(typeHash, *this);
	}
}

void MessageRouter::Subscribe(const size_t TypeHash, std::function<void(const Dispatchable&)>&& Callback)
{
	mySubscribedMessages.insert(TypeHash);
	DataRouters::Get().GetRouter(myThreadId).SubscribeToType(TypeHash, *this, std::move(Callback));
}
