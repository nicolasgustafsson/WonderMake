#pragma once

#include "WonderMakeUtility/RestrictTypes.h"

#include <functional>
#include <unordered_set>

class Dispatchable;

template<typename TMessage, typename TThis, typename ...TArgs>
static std::function<void(const TMessage&)> BindHelper(void (TThis::*aFunctionPtr)(const TMessage&, TArgs&&...), TThis* aThis, TArgs... aArgs)
{
	return [aFunctionPtr, aThis, ...args = std::forward<TArgs>(aArgs)](const TMessage& aMessage)
	{
		(aThis->*aFunctionPtr)(aMessage, std::forward<TArgs>(args)...);
	};
};

template<typename TMessage, typename TThis, typename ...TArgs>
static std::function<void(const TMessage&)> BindHelper(void (TThis::*aFunctionPtr)(const TMessage&, TArgs&&...) const, TThis* aThis, TArgs... aArgs)
{
	return [aFunctionPtr, aThis, ...args = std::forward<TArgs>(aArgs)](const TMessage& aMessage)
	{
		(aThis->*aFunctionPtr)(aMessage, std::forward<TArgs>(args)...);
	};
};

template<typename TMessage, typename ...TArgs>
static std::function<void(const TMessage&)> BindHelper(void (*aFunctionPtr)(const TMessage&, TArgs&&...), TArgs... aArgs)
{
	return[aFunctionPtr, ...args = std::forward<TArgs>(aArgs)](const TMessage& aMessage)
	{
		(*aFunctionPtr)(aMessage, std::forward<TArgs>(args)...);
	};
};

class MessageSubscriber final
	: private NonCopyable
{
public:
	MessageSubscriber() = default;

	template<typename... TRoutes>
	MessageSubscriber(TRoutes... aRoutes)
	{
		(AddRoute(std::move(aRoutes)), ...);
	}
	~MessageSubscriber();

	template<typename TMessage>
	void AddRoute(std::function<void(const TMessage&)> aCallback)
	{
		Subscribe(TMessage::GetTypeHash(), [callback = std::move(aCallback)](const Dispatchable& aMessage) mutable
		{
			MessageSubscriber::RouteDelegate<TMessage>(callback, aMessage);
		});
	}

	template<typename TMessage>
	void RemoveRoute()
	{
		RemoveRoute(TMessage::GetTypeHash());
	}

	void RemoveRoute(const size_t aTypeHash);

private:
	template<typename TMessage, typename TCallable> requires std::is_invocable_v<TCallable, const TMessage&>
	static void RouteDelegate(TCallable& aCallback, const Dispatchable& aMessage)
	{
		aCallback(static_cast<const TMessage&>(aMessage));
	}

	void Subscribe(const size_t aTypeHash, std::function<void(const Dispatchable&)> aCallback);

	std::unordered_set<size_t> mySubscribedMessages;
};
