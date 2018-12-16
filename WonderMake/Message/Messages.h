#pragma once

#include "MessageTypes.h"
#include "DispatchableBuffer.h"

template<typename TMessage>
inline static void WmDispatchMessage(const TMessage& aMessage)
{
	DispatchableBuffer<TMessage>::Dispatch(aMessage);
}

template<typename TMessage, typename = std::enable_if_t<!std::is_lvalue_reference_v<TMessage>>>
inline static void WmDispatchMessage(TMessage&& aMessage)
{
	DispatchableBuffer<TMessage>::Dispatch(std::move(aMessage));
}

template<typename TMessage>
inline static void WmDispatchMessage(TMessage&& aMessage, const EThreadId aThreadId)
{
	DispatchableBuffer<std::decay<TMessage>::type>::Dispatch(std::move(aMessage), aThreadId);
}

template<typename TMessage, typename = std::enable_if_t<!std::is_lvalue_reference_v<TMessage>>>
inline static void WmDispatchMessage(const TMessage& aMessage, const EThreadId aThreadId)
{
	DispatchableBuffer<std::decay<TMessage>::type>::Dispatch(aMessage, aThreadId);
}

inline static void WmDispatchTask(Task&& aTask, const EThreadId aThreadId)
{
	DispatchableBuffer<Task>::Dispatch(std::move(aTask), aThreadId);
}

inline static void WmDispatchTask(const Task& aTask, const EThreadId aThreadId)
{
	DispatchableBuffer<Task>::Dispatch(aTask, aThreadId);
}

inline static void WmDispatchTask(Closure&& aTask, const EThreadId aThreadId)
{
	DispatchableBuffer<Task>::Dispatch(std::move(Task(aTask)), aThreadId);
}

inline static void WmDispatchTask(const Closure& aTask, const EThreadId aThreadId)
{
	DispatchableBuffer<Task>::Dispatch(std::move(Task(aTask)), aThreadId);
}
