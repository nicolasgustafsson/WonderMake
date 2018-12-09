#include "stdafx.h"
#include "DispatchRouter.h"

#include "MessageTypes.h"

void DispatchRouter::Dispatch(std::unique_ptr<Dispatchable>&& DispatchedMessage)
{
	myDispatchBuffer.Add(std::move(DispatchedMessage));
}

void DispatchRouter::FlushMessagesAndRunTasks()
{
	if (!myDispatchBuffer.ClearAndSwap())
	{
		return;
	}
	
	const auto& readBuffer = myDispatchBuffer.GetReadBuffer();
	for (const auto& dispatch : readBuffer)
	{
		if (dispatch->GetTypeHash() == Task::GetTypeHash())
		{
			RunTask(static_cast<const Task&>(*dispatch));
			continue;
		}
		RouteMessage(*dispatch);
	}
}

void DispatchRouter::SubscribeToType(const size_t TypeHash, const MessageRouter& Subscriber, std::function<void(const Dispatchable&)>&& Callback)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myRouters[TypeHash].emplace_back(std::make_pair(&Subscriber, Callback));
}

void DispatchRouter::UnsubscribeToType(const size_t TypeHash, const MessageRouter& Subscriber)
{
	std::lock_guard<decltype(myLock)> lock(myLock);

	auto it = myRouters.find(TypeHash);
	if (it == myRouters.end())
	{
		// TODO(Kevin): Log error
		return;
	}

	auto subscriberIt = std::find_if(it->second.begin(), it->second.end(), [&Subscriber](const auto& Element)
	{
		return Element.first == &Subscriber;
	});

	if (subscriberIt == it->second.end())
	{
		// TODO(Kevin): Log error
		return;
	}

	it->second.erase(subscriberIt);
}

void DispatchRouter::RunTask(const Task& Job)
{
	Job.Run();
}

void DispatchRouter::RouteMessage(const Dispatchable& DispatchedMessage)
{
	std::lock_guard<decltype(myLock)> lock(myLock);

	auto it = myRouters.find(DispatchedMessage.GetTypeHash());
	if (it == myRouters.end())
	{
		return;
	}

	for (const auto& subscriberIt : it->second)
	{
		subscriberIt.second(DispatchedMessage);
	}
}
