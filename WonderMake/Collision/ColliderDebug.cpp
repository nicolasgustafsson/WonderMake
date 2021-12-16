#include "pch.h"

#include "ColliderDebug.h"
#include "Colliders.h"

void DrawSphere(const Colliders::SSphere& aCollider, const SColor& aColor)
{
	WmDrawDebugCircle({ aCollider.Position, aCollider.Radius }, aColor, 4 + static_cast<u32>(aCollider.Radius) / 10);
}

void DrawLine(const Colliders::SCollisionLine& aCollider, const SColor& aColor)
{
	SDebugLine line;
	line.Color = aColor;

	line.Start = aCollider.Position;
	line.End = aCollider.GetLineEnd();
	WmDrawDebugLine(line);
}

void DrawCollider(const Colliders::Shape& aCollider, const SColor& aColor)
{
	std::visit([aColor](const auto& aCollider)
		{
			using T = std::decay_t<decltype(aCollider)>;

			if constexpr (std::is_same_v<T, Colliders::SSphere>)
			{
				DrawSphere(aCollider, aColor);
			}
			else if constexpr (std::is_same_v<T, Colliders::SCollisionLine>)
			{
				DrawLine(aCollider, aColor);
			}
			else
			{
				//static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}
