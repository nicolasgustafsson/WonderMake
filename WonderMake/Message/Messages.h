#pragma once

#include "Message/MessageTypes.h"
#include "Message/DispatchableBuffer.h"

#include "Debugging/DebugLine.h"

#include "Utilities/OS.h"

#include <sstream>
#include <memory>

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

	MessageStream << '[' << GetDateTime() << ']';

	(MessageStream << ... << aMessageArgs);

	WmDispatchMessage(SLogMessage(std::forward<std::string>(MessageStream.str())));
}

inline static void WmDrawDebugLine(const SDebugLine& aDebugLine)
{
	WmDispatchMessage(SDebugLineMessage(aDebugLine));
}

inline static void WmDrawDebugLine(const SVector2f& aStart, const SVector2f& aEnd, const SColor& aColor, const f32 aDuration = 0.0f)
{
	SDebugLine line{ aColor, aStart, aEnd, aDuration };

	WmDrawDebugLine(line);
}

class Job;

template<typename TFunctionPtr, typename TCaller, typename ...TArgs>
inline static void _RunTask(TFunctionPtr aFunctionPtr, TCaller* const aCaller, TArgs... aArgs)
{
	if constexpr (std::is_base_of<Job, TCaller>::value)
	{
		if (aCaller->IsComplete())
		{
			return;
		}
	}
	(aCaller->*aFunctionPtr)(aArgs...);
}

template<typename TFunctionPtr, typename TCaller, typename ...TArgs>
inline static auto BindTask(TFunctionPtr aFunctionPtr, TCaller* const aCaller, TArgs... aArgs)
{
	return[aFunctionPtr, aCaller, aArgs...]
	{
		_RunTask(aFunctionPtr, aCaller, aArgs...);
	};
}

template<typename TFunctionPtr, typename TCaller, typename ...TArgs>
inline static auto BindTask(TFunctionPtr aFunctionPtr, std::unique_ptr<TCaller> aCaller, TArgs... aArgs)
{
	return[aFunctionPtr, aCaller = std::move(aCaller), aArgs...]
	{
		_RunTask(aFunctionPtr, aCaller.get(), aArgs...);
	};
}

template<typename TFunctionPtr, typename TCaller, typename ...TArgs>
inline static auto BindTask(TFunctionPtr aFunctionPtr, std::shared_ptr<TCaller> aCaller, TArgs... aArgs)
{
	return[aFunctionPtr, aCaller = std::move(aCaller), aArgs...]
	{
		_RunTask(aFunctionPtr, aCaller.get(), aArgs...);
	};
}

template<typename TFunctionPtr, typename TCaller, typename ...TArgs>
inline static auto BindTask(TFunctionPtr aFunctionPtr, std::weak_ptr<TCaller> aCaller, TArgs... aArgs)
{
	return[aFunctionPtr, aCaller, aArgs...]
	{
		auto caller = aCaller.lock();
		if (!caller)
		{
			return;
		}
		_RunTask(aFunctionPtr, caller.get(), aArgs...);
	};
}
