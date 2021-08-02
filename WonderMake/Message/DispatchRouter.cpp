#include "pch.h"
#include "DispatchRouter.h"

#include "Message/MessageTypes.h"

void DispatchRouter::RouteDispatchable(const Dispatchable& aDispatchedMessage)
{
	auto it = mySubscribers.find(aDispatchedMessage.GetTypeHash());
	if (it == mySubscribers.end())
	{
		return;
	}

	for (auto& subscriberIt : it->second)
	{
		if (std::find(myNewUnsubscriptions.begin(), myNewUnsubscriptions.end(), subscriberIt.mySubscription) != myNewUnsubscriptions.end())
		{
			continue;
		}
		subscriberIt.myCallback(aDispatchedMessage);
	}
}
void DispatchRouter::CommitChanges()
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	for (auto& subscription : myNewSubscriptions)
	{
		mySubscribers[subscription.mySubscription.myTypeHash].emplace_back(std::move(subscription));
	}
	myNewSubscriptions.clear();

	for (const auto& subscription : myNewUnsubscriptions)
	{
		auto it = mySubscribers.find(subscription.myTypeHash);
		if (it == mySubscribers.end())
		{
			// TODO(Kevin): Log error
			return;
		}

		auto subscriberIt = std::find_if(it->second.begin(), it->second.end(), [&subscription](const auto& aElement)
		{
			return aElement.mySubscription.mySubscriber == subscription.mySubscriber;
		});

		if (subscriberIt == it->second.end())
		{
			// TODO(Kevin): Log error
			return;
		}

		it->second.erase(subscriberIt);
	}
	myNewUnsubscriptions.clear();
}

void DispatchRouter::SubscribeToType(const size_t aTypeHash, const MessageSubscriber& aSubscriber, UniqueFunction<void(const Dispatchable&)> aCallback)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myNewSubscriptions.emplace_back(SSubscriptionOrder{ std::move(aCallback), { &aSubscriber, aTypeHash } });
}

void DispatchRouter::UnsubscribeToType(const size_t aTypeHash, const MessageSubscriber& aSubscriber)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myNewUnsubscriptions.emplace_back(SSubscription{ &aSubscriber, aTypeHash });
}
