#pragma once

#include "Collision/CollisionComponent.h"

#include "Utilities/Debugging/Debugged.h"

#include "Functionalities/Functionality.h"
#include "Functionalities/TransformFunctionality.h"

namespace Colliders
{
	struct SSphere;
}

class CollisionFunctionality final
	: public Functionality<
		CollisionFunctionality,
		SCollisionComponent,
		TransformFunctionality>
	, public Debugged
{
public:

	CollisionFunctionality(Object& aOwner) noexcept;
	~CollisionFunctionality();

	void Tick();

	void AddSphereCollider(const SVector2f aOffset, const f32 aRadius);

	void Debug() override;

	// TODO(Kevin): Allow objects to react to collisions.
	
private:
	
};
