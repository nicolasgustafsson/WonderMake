#pragma once

#include "MessageTypes.h"

#include "Threads/DoubleBuffer.h"

#include "Utilities/RestrictTypes.h"

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

class MessageRouter;

class DispatchRouter final
	: private NonCopyable
{
public:
	DispatchRouter() = default;

	void Dispatch(std::unique_ptr<Dispatchable>&& DispatchedMessage);
	void FlushMessagesAndRunTasks();

	void SubscribeToType(const size_t TypeHash, const MessageRouter& Subscriber, std::function<void(const Dispatchable&)>&& Callback);
	void UnsubscribeToType(const size_t TypeHash, const MessageRouter& Subscriber);

private:
	void RunTask(const Task& Job);
	void RouteMessage(const Dispatchable& Message);

	DoubleBuffer<std::unique_ptr<Dispatchable>> myDispatchBuffer;
	std::unordered_map<size_t, std::vector<std::pair<const MessageRouter*, std::function<void(const Dispatchable&)>>>> myRouters;
	std::mutex myLock;
};
