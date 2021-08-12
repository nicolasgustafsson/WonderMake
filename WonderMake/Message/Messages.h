#pragma once

#include "Message/MessageTypes.h"
#include "Message/DispatchableBuffer.h"

#include "Debugging/DebugLine.h"

#include "Utilities/Platform.h"

#include <sstream>
#include <memory>
#include "Utilities/BezierCurve.h"
#include "Utilities/Geometry.h"


template<typename TMessage>
inline static void WmDispatchMessage(const TMessage& aMessage)
{
	DispatchableBuffer::Get().Dispatch(aMessage);
}

template<typename TMessage, typename = std::enable_if_t<!std::is_lvalue_reference_v<TMessage>>>
inline static void WmDispatchMessage(TMessage&& aMessage) noexcept
{
	DispatchableBuffer::Get().Dispatch(std::move(aMessage));
}

template<typename ... TMessageArgs>
inline static void WmLog(TMessageArgs... aMessageArgs)
{
	std::stringstream MessageStream;

	MessageStream << '[' << Platform::GetDateTime() << ']';

	(MessageStream << ... << aMessageArgs);

	WmDispatchMessage(SLogMessage(MessageStream.str()));
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

inline static void WmDrawDebugRectangle(const SRectangle& aRectangle, const SColor& aColor, const f32 aDuration = 0.0f)
{
	WmDrawDebugLine({ aRectangle.Left , aRectangle.Top }, { aRectangle.Left, aRectangle.Bottom }, aColor, aDuration);
	WmDrawDebugLine({ aRectangle.Right , aRectangle.Top }, { aRectangle.Right, aRectangle.Bottom }, aColor, aDuration);
	WmDrawDebugLine({ aRectangle.Left , aRectangle.Top }, { aRectangle.Right, aRectangle.Top }, aColor, aDuration);
	WmDrawDebugLine({ aRectangle.Left , aRectangle.Bottom }, { aRectangle.Right, aRectangle.Bottom }, aColor, aDuration);
}

inline static void WmDrawDebugBezier(const BezierCurve& aCurve, const SColor& aColor, const i32 aSegments = 10, const f32 aDuration = 0.0f)
{
	SVector2f previousLocation = aCurve.GetConstantLocationAt(0.f);
	for (i32 i = 1; i <= aSegments - 1; i++)
	{
		const SVector2f location = aCurve.GetConstantLocationAt(static_cast<f32>(i) / static_cast<f32>(aSegments));
		WmDrawDebugLine(previousLocation, location, aColor, aDuration);
		previousLocation = location;
	}
}
