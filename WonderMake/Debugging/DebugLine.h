#pragma once
#include "Message/MessageTypes.h"

#include "Utilities/Color.h"
#include "Utilities/Vector.h"

struct SDebugLine
{
public:
	SColor Color;
	SVector2f Start;
	SVector2f End;
	f32 Duration = 0.f;
};

struct SDebugLineMessage
	: public Message<SDebugLineMessage>
{
	SDebugLineMessage(const SDebugLine& aDebugLine) noexcept
		: Line(aDebugLine) {}

	SDebugLine Line;
};