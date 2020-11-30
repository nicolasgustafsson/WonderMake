#pragma once
#include "Functionalities/Functionality.h"
#include "Components/TransformComponent.h"

#include "Utilities/Matrix.h"

class TransformFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<STransformComponent2D, PWrite>>>
{
public:
	void SetPosition(const SVector2f aPosition) noexcept;
	[[nodiscard]] SVector2f GetPosition() const noexcept;

	void FacePosition(const SVector2f aPosition) noexcept;
	void FaceDirection(const SVector2f aDirection) noexcept;

	template<typename TRotation>
		requires std::is_floating_point_v<TRotation>
	void SetRotation(const TRotation aRotation) noexcept
	{
		Get<STransformComponent2D>().Rotation = aRotation;
	}
	template<typename TRotation>
		requires !std::is_floating_point_v<TRotation>
	void SetRotation(const TRotation aRotation) noexcept
	{
		Get<STransformComponent2D>().Rotation = RotationCast<STransformComponent2D::RotationType>(aRotation);
	}
	template<typename TReturnRotation = SDegreeF32>
	[[nodiscard]] TReturnRotation GetRotation() const noexcept
	{
		return RotationCast<TReturnRotation>(Get<STransformComponent2D>().Rotation);
	}

	[[nodiscard]] SVector2f GetForwardVector() const noexcept;
	[[nodiscard]] SVector2f GetRightVector() const noexcept;

	void Move(const SVector2f aMovement) noexcept;

	[[nodiscard]] SMatrix33f GetMatrix() const noexcept;

	void Inspect();

private:
};
