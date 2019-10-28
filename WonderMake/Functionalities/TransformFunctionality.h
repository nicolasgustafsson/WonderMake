#pragma once
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"

#include "Utilities/Matrix33.h"

class TransformFunctionality
	: public Functionality<TransformFunctionality, STransformComponent>
{
public:
	TransformFunctionality(Object& aOwner) noexcept;

	void SetPosition(const SVector2f aPosition) noexcept;
	[[nodiscard]] SVector2f GetPosition() const noexcept;

	void FacePosition(const SVector2f aPosition) noexcept;

	void FaceDirection(const SVector2f aDirection) noexcept;

	void SetRotation(const f32 aRotation) noexcept;
	[[nodiscard]] f32 GetRotation() const noexcept;

	SVector2f GetForwardVector() const noexcept;

	void Move(const SVector2f aMovement) noexcept;

	[[nodiscard]] SMatrix33f GetMatrix() const noexcept;

	void Inspect();

private:
};
