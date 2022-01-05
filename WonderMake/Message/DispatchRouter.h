#pragma once

#include "WonderMakeBase/RestrictTypes.h"
#include "WonderMakeBase/Singleton.h"
#include "WonderMakeBase/UniqueFunction.h"

#include <mutex>
#include <unordered_map>
#include <vector>

class Dispatchable;
class MessageSubscriber;

class DispatchRouter final
	: private NonCopyable
	, public Singleton<DispatchRouter>
{
public:
	void RouteDispatchable(const Dispatchable& aDispatchedMessage);
	void CommitChanges();

	void SubscribeToType(const size_t aTypeHash, const MessageSubscriber& aSubscriber, UniqueFunction<void(const Dispatchable&)> aCallback);
	void UnsubscribeToType(const size_t aTypeHash, const MessageSubscriber& aSubscriber);

private:
	struct SSubscription
	{
		const MessageSubscriber* mySubscriber;
		size_t myTypeHash;
	};

	struct SSubscriptionOrder
	{
		UniqueFunction<void(const Dispatchable&)> myCallback;
		SSubscription mySubscription;
	};

	std::vector<SSubscriptionOrder> myNewSubscriptions;
	std::vector<SSubscription> myNewUnsubscriptions;

	std::unordered_map<size_t, std::vector<SSubscriptionOrder>> mySubscribers;
	std::mutex myLock;
};
