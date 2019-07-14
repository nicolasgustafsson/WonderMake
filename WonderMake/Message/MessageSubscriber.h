#pragma once

#include "Threads/RoutineIds.h"

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
	template<typename ...ROUTES>
	MessageSubscriber(const ERoutineId aRoutineId);

	template<typename ...ROUTES>
	MessageSubscriber(const ERoutineId aRoutineId, ROUTES... aRoutes);
	~MessageSubscriber();

	template<typename T>
	void AddRoute(std::function<void(const T&)> aCallback); 

	template<typename T>
	void RemoveRoute();

	void RemoveRoute(const size_t aTypeHash);

private:
	template<typename T>
	static void RouteDelegate(const std::function<void(const T&)> aCallback, const Dispatchable& aMessage);

	void Subscribe(const size_t aTypeHash, std::function<void(const Dispatchable&)>&& aCallback);

	std::unordered_set<size_t> mySubscribedMessages;
	ERoutineId myRoutineId;
};

template<typename T>
void MessageSubscriber::RemoveRoute()
{
	RemoveRoute(T::GetTypeHash());
}

template<typename ...ROUTES>
MessageSubscriber::MessageSubscriber(const ERoutineId aRoutineId)
	: myRoutineId(aRoutineId)
{
}


template<typename ...ROUTES>
MessageSubscriber::MessageSubscriber(const ERoutineId aRoutineId, ROUTES... aRoutes)
	: MessageSubscriber(aRoutineId)
{
	(AddRoute(std::move(aRoutes)), ...);
}

template<typename T>
void MessageSubscriber::RouteDelegate(const std::function<void(const T&)> aCallback, const Dispatchable& aMessage)
{
	aCallback(static_cast<const T&>(aMessage));
}

template<typename T>
void MessageSubscriber::AddRoute(std::function<void(const T&)> aCallback)
{
	Subscribe(T::GetTypeHash(), std::bind(&MessageSubscriber::RouteDelegate<T>, std::move(aCallback), std::placeholders::_1));
}
