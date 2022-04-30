#pragma once

#include "Message/MessageTypes.h"
#include "Message/DispatchableBuffer.h"

#include "Debugging/DebugLine.h"

#include "Utilities/Platform.h"

#include <sstream>
#include <memory>

#include "Constants.h"
#include "Universe/UniverseHelper.h"
#include "Universe/UniverseManagerSystem.h"
#include "Utilities/Geometry/BezierCurve.h"
#include "Utilities/Geometry/Geometry.h"
#include "Utilities/Container/Container.h"
#include "Utilities/ToString.h"


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

template <typename ... TMessageArgs>
inline static void WmLog(TMessageArgs... aMessageArgs)
{
	std::stringstream MessageStream;

	MessageStream << '[' << Platform::GetDateTime() << ']';

	(MessageStream << ... << aMessageArgs);

	WmDispatchMessage(SLogMessage(MessageStream.str()));
}

inline static void WmDrawDebugLine(const SDebugLine& aDebugLine)
{
	WmDispatchMessage(SDebugLineMessage{ aDebugLine, GetCurrentUniverseId() });
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

inline static void WmDrawDebugBezier(const BezierCurve& aCurve, const SColor& aColor, const u32 aSegments = 10, const f32 aDuration = 0.0f)
{
	SVector2f previousLocation = aCurve.GetConstantLocationAt(0.f);
	for (u32 i = 1; i <= aSegments - 1; i++)
	{
		const SVector2f location = aCurve.GetConstantLocationAt(static_cast<f32>(i) / static_cast<f32>(aSegments));
		WmDrawDebugLine(previousLocation, location, aColor, aDuration);
		previousLocation = location;
	}
}

inline static void WmDrawDebugCircle(const SCircle& aCircle, const SColor& aColor, const u32 aSegments = 10, const f32 aDuration = 0.0f)
{
	Container<SVector2f> positions;

	for (u32 i = 0; i < aSegments; ++i)
	{
		positions.Add(SVector2f{});
		positions[i].X = std::cos(((Constants::Pi * 2.f) / (aSegments )) * i);
		positions[i].Y = std::sin(((Constants::Pi * 2.f) / (aSegments )) * i);

		positions[i] *= aCircle.Radius;
		positions[i] += aCircle.Position;
	}

	SDebugLine line;
	line.Color = aColor;
	line.Duration = aDuration;

	for (u32 i = 0; i < aSegments - 1; ++i)
	{
		line.Start = positions[i];
		line.End = positions[i + 1];

		WmDrawDebugLine(line);
	}


	line.Start = positions[0];
	line.End = positions[aSegments - 1];

	WmDrawDebugLine(line);
}