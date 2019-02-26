#pragma once
#include "Message/MessageTypes.h"

struct SDebugLine
{
public:
	SColor Color;
	SVector2f Start;
	SVector2f End;
	f32 Duration;
};

struct SDebugLineMessage
	: public Message<SDebugLineMessage>
{
	SDebugLineMessage(const SDebugLine& aDebugLine) noexcept
		: Line(aDebugLine) {}

	SDebugLine Line;
};