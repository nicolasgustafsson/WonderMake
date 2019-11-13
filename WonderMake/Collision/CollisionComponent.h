#pragma once

#include "Collision/Colliders.h"

#include "Components/Component.h"

#include "Serialization/Serialization.h"

#include <vector>

struct SCollider
{
	SVector2f				Offset;
	Colliders::Shape*		Collider = nullptr;
	/*
	void SerializeCollider(const Colliders::Shape& aCollider)
	{

	}

	const Colliders::Shape& DeserializeCollider() const
	{
		return name;
	}
	*/
};

struct SCollisionComponent final
	: public SComponent
{
	std::vector<SCollider>	Colliders;
};

namespace meta
{
	template <>
	inline auto registerMembers<SCollider>()
	{
		return members(
			member("Offset", &SCollider::Offset)
		);
	}

	template <>
	inline auto registerMembers<SCollisionComponent>()
	{
		return members(
			member("Colliders", &SCollisionComponent::Colliders)
		);
	}
}
