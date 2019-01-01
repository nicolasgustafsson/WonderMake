#pragma once

#include "MessageTypes.h"
#include "DispatchableBuffer.h"
#include <sstream>

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
inline static void WmDispatchMessage(TMessage&& aMessage, const ERoutineId aRoutineId)
{
	DispatchableBuffer<std::decay<TMessage>::type>::Dispatch(std::move(aMessage), aRoutineId);
}

template<typename TMessage, typename = std::enable_if_t<!std::is_lvalue_reference_v<TMessage>>>
inline static void WmDispatchMessage(const TMessage& aMessage, const ERoutineId aRoutineId)
{
	DispatchableBuffer<std::decay<TMessage>::type>::Dispatch(aMessage, aRoutineId);
}

inline static void WmDispatchTask(Task&& aTask, const ERoutineId aRoutineId)
{
	DispatchableBuffer<Task>::Dispatch(std::move(aTask), aRoutineId);
}

inline static void WmDispatchTask(const Task& aTask, const ERoutineId aRoutineId)
{
	DispatchableBuffer<Task>::Dispatch(aTask, aRoutineId);
}

inline static void WmDispatchTask(Closure&& aTask, const ERoutineId aRoutineId)
{
	DispatchableBuffer<Task>::Dispatch(std::move(Task(aTask)), aRoutineId);
}

inline static void WmDispatchTask(const Closure& aTask, const ERoutineId aRoutineId)
{
	DispatchableBuffer<Task>::Dispatch(std::move(Task(aTask)), aRoutineId);
}

template<typename ... TMessageArgs>
inline static void WmLog(TMessageArgs... aMessageArgs)
{
	std::stringstream MessageStream;

	(MessageStream << ... << aMessageArgs);

	WmDispatchMessage(SLogMessage(std::forward<std::string>(MessageStream.str())));
}
