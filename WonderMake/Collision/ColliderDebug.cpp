#include "pch.h"

#include "ColliderDebug.h"
#include "Colliders.h"

void DrawSphere(const Colliders::SSphere& aCollider, const SColor& aColor)
{
	//WmDrawDebugSphere(aCollider.Position, aCollider.Radius, aColor);
}

void DrawLine(const Colliders::SLine& aCollider, const SColor& aColor)
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
			else if constexpr (std::is_same_v<T, Colliders::SLine>)
			{
				DrawLine(aCollider, aColor);
			}
			else
			{
				static_assert(std::false_type::value, "Collider not implemented!");
			}
		}, aCollider);
}
