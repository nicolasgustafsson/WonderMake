#pragma once
#include "Message/MessageTypes.h"

#include "Utilities/Color.h"
#include "Utilities/Id.h"
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
	SDebugLineMessage(const SDebugLine& aDebugLine, const Id aUniverseId) noexcept
		: Line(aDebugLine), UniverseId(aUniverseId) {}

	SDebugLine Line;
	Id UniverseId;
};