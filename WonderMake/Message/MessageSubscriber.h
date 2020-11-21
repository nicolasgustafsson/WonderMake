#pragma once

#include "Utilities/RestrictTypes.h"

#include <functional>
#include <unordered_set>

class Dispatchable;

template<typename T, typename U, typename ...ARGS>
static std::function<void(const T&)> BindHelper(void (U::*aFunctionPtr)(const T&, ARGS...), U* aThis, ARGS... aArgs)
{
	return std::bind(aFunctionPtr, aThis, std::placeholders::_1, aArgs...);
};

template<typename T, typename U, typename ...ARGS>
static std::function<void(const T&)> BindHelper(void (U::*aFunctionPtr)(const T&, ARGS...) const, U* aThis, ARGS... aArgs)
{
	return std::bind(aFunctionPtr, aThis, std::placeholders::_1, aArgs...);
};

template<typename T, typename ...ARGS>
static std::function<void(const T&)> BindHelper(void (*aFunctionPtr)(const T&, ARGS...), ARGS... aArgs)
{
	return std::bind(aFunctionPtr, std::placeholders::_1, aArgs...);
};

class MessageSubscriber final
	: private NonCopyable
{
public:
	MessageSubscriber() = default;

	template<typename ...ROUTES>
	MessageSubscriber(ROUTES... aRoutes)
	{
		(AddRoute(std::move(aRoutes)), ...);
	}
	~MessageSubscriber();

	template<typename T>
	void AddRoute(std::function<void(const T&)> aCallback)
	{
		Subscribe(T::GetTypeHash(), std::bind(&MessageSubscriber::RouteDelegate<T>, std::move(aCallback), std::placeholders::_1));
	}

	template<typename T>
	void RemoveRoute()
	{
		RemoveRoute(T::GetTypeHash());
	}

	void RemoveRoute(const size_t aTypeHash);

private:
	template<typename T>
	static void RouteDelegate(const std::function<void(const T&)> aCallback, const Dispatchable& aMessage)
	{
		aCallback(static_cast<const T&>(aMessage));
	}

	void Subscribe(const size_t aTypeHash, std::function<void(const Dispatchable&)>&& aCallback);

	std::unordered_set<size_t> mySubscribedMessages;
};
