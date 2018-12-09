#pragma once

#include "Threads/ThreadIds.h"

#include "Utilities/RestrictTypes.h"

#include <functional>
#include <unordered_set>

class Dispatchable;

template<typename T, typename U, typename ...ARGS>
static std::function<void(const T&)> BindHelper(void (U::*FunctionPtr)(const T&, ARGS...), U* This, ARGS... Args)
{
	return std::bind(FunctionPtr, This, std::placeholders::_1, Args...);
};

template<typename T, typename ...ARGS>
static std::function<void(const T&)> BindHelper(void (*FunctionPtr)(const T&, ARGS...), ARGS... Args)
{
	return std::bind(FunctionPtr, std::placeholders::_1, Args...);
};

class MessageRouter final
	: private NonCopyable
{
public:
	template<typename ...ROUTES>
	MessageRouter(const EThreadId ThreadId, ROUTES... Routes);
	~MessageRouter();

private:
	template<typename T>
	static void RouteDelegate(std::function<void(const T&)>&& Callback, const Dispatchable& Message);
	template<typename T>
	void SetRoute(std::function<void(const T&)>&& Callback);

	void Subscribe(const size_t TypeHash, std::function<void(const Dispatchable&)>&& Callback);

	std::unordered_set<size_t> mySubscribedMessages;
	EThreadId myThreadId;
};

template<typename ...ROUTES>
MessageRouter::MessageRouter(const EThreadId ThreadId, ROUTES... Routes)
	: myThreadId(ThreadId)
{
	(SetRoute(std::move(Routes)), ...);
}

template<typename T>
void MessageRouter::RouteDelegate(std::function<void(const T&)>&& Callback, const Dispatchable& Message)
{
	Callback(static_cast<const T&>(Message));
}

template<typename T>
void MessageRouter::SetRoute(std::function<void(const T&)>&& Callback)
{
	Subscribe(T::GetTypeHash(), std::bind(&MessageRouter::RouteDelegate<T>, std::move(Callback), std::placeholders::_1));
}
