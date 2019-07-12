#include "pch.h"

#include "ColliderDebug.h"
#include "Colliders.h"

void DrawCollider(const Colliders::SBase& aCollider, SColor aColor)
{
	switch (aCollider.Type)
	{
	case Colliders::Type::Sphere:
		DrawCollider(static_cast<const Colliders::SSphere&>(aCollider), aColor);

		break;
	}
}

void DrawCollider(const Colliders::SSphere& aCollider, SColor aColor)
{
	constexpr u32 points = 16;
	SVector2f positions[points];

	for (u32 i = 0; i < points; ++i)
	{
		positions[i].X = std::cosf((3.14f * 2.f) / points * i);
		positions[i].Y = std::sinf((3.14f * 2.f) / points * i);

		positions[i] *= aCollider.Radius;
		positions[i] += aCollider.Position;
	}

	SDebugLine line;
	line.Color = SColor::Red;

	for (u32 i = 0; i < points - 1; ++i)
	{
		line.Start = positions[i];
		line.End = positions[i + 1];

		WmDrawDebugLine(line);
	}

	line.Start = positions[points - 1];
	line.End = positions[0];

	WmDrawDebugLine(line);
}
