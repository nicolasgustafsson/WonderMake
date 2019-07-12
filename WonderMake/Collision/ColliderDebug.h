#pragma once

#include "Utilities/Color.h"

namespace Colliders
{
	struct SBase;
	struct SSphere;
}

void DrawCollider(const Colliders::SBase& aCollider, SColor aColor = SColor::Red);
void DrawCollider(const Colliders::SSphere& aCollider, SColor aColor = SColor::Red);
