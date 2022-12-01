#include "pch.h"

#include "ColliderDebug.h"
#include "Colliders.h"

void DrawSphere(const Colliders::SSphere& aCollider, const SColor& aColor)
{
	constexpr u32 points = 16;
	SVector2f positions[points];

	for (u32 i = 0; i < points; ++i)
	{
		positions[i].X = std::cosf((Constants::Pi * 2.f) / points * i);
		positions[i].Y = std::sinf((Constants::Pi * 2.f) / points * i);

		positions[i] *= aCollider.Radius;
		positions[i] += aCollider.Position;
	}

	SDebugLine line;
	line.Color = aColor;

	for (u32 i = 0; i < points - 1; ++i)
	{
		line.Start = positions[i];
		line.End = positions[i + 1];

		WmDrawDebugLine(line);
	}

	line.Start = positions[points - 1];
	line.End = positions[0];

	//WmDrawDebugLine(line);
}

void DrawAABB(const Colliders::SAxisAlignedBoundingBox& aCollider, const SColor& aColor)
{
	static constexpr u32 points = 5;

	SVector2f positions[points] =
	{
		aCollider.Position,
		{ aCollider.Position.X + aCollider.Dimensions.X, aCollider.Position.Y },
		aCollider.Position + aCollider.Dimensions,
		{ aCollider.Position.X, aCollider.Position.Y + aCollider.Dimensions.Y },
		aCollider.Position
	};

	SDebugLine line;
	line.Color = aColor;

	for (u32 i = 0; i < points - 1; ++i)
	{
		line.Start = positions[i];
		line.End = positions[i + 1];

		WmDrawDebugLine(line);
	}
}

void DrawLine(const Colliders::SCollisionLine& aCollider, const SColor& aColor)
{
	SDebugLine line;
	line.Color = aColor;

	line.Start = aCollider.Position;
	line.End = aCollider.GetLineEnd();
	//WmDrawDebugLine(line);
}

void DrawCollider(const Colliders::Shape& aCollider, const SColor& aColor)
{
	std::visit([aColor](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
				DrawSphere(aCollider, aColor);
			else if constexpr (std::is_same_v<T, Colliders::SAxisAlignedBoundingBox>)
				DrawAABB(aCollider, aColor);
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
				DrawLine(aCollider, aColor);
			else
				static_assert(std::false_type::value, "Collider not implemented!");

		}, aCollider);
}
